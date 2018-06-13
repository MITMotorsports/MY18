#include "page_manager.h"

#include "board.h"
#include "NHD_US2066_charset.h"

#define DESIRED_VOLTAGE mc_voltage
#define DATA_UNKNOWN "?"

void page_manager_init(page_manager_t *pm, carstats_t *stats) {
    pm->page  = DASH_PAGE_CRITICAL;
    pm->stats = stats;
}

void page_manager_next_page(page_manager_t *pm) {
    // wrap around
    pm->page = (pm->page + 1) % DASH_PAGE_COUNT;

    // switch (pm->page) {
        // case DASH_PAGE_CRITICAL:
            // pm->page = DASH_PAGE_TRACTION;
            // break;
        // case DASH_PAGE_TRACTION:
            // pm->page = DASH_PAGE_FAULT;
            // break;
        // default:
            // pm->page = DASH_PAGE_CRITICAL;
            // break;
    // }
}

void page_manager_prev_page(page_manager_t *pm) {
    if (pm->page - 1 < 0) {
        pm->page = DASH_PAGE_COUNT-1;
    } else {
        pm->page -= 1;
    }
}

void page_manager_set_page(page_manager_t *pm, dash_page_type page) {
    pm->page = page;
}

/*
const char *page_type_repr(dash_page_type page) {
    switch (page) {
        case DASH_PAGE_CRITICAL:
            return "critical";
        case DASH_PAGE_TRACTION:
            return "traction";
        case DASH_PAGE_WHEEL_SPEED:
            return "speed";
        default:
            return "";
    }
}*/

void draw_nav_line(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_clearline(oled, 3);
    oled_set_pos(oled, 3, 0);
    if (pm->page - 1 >= 0) {
        oled_clear(oled);
        oled_print_char(oled, CHAR_ARROW_LEFT);
        char *pagename = (char*) page_type_repr(pm->page - 1);
        oled_print(oled, pagename);
    }

    if (pm->page + 1 < DASH_PAGE_COUNT) {
        oled_clear(oled);
        char *pagename = (char*) page_type_repr(pm->page + 1);
        oled_rprint_pad(oled, pagename, 1);
        oled_print_char(oled, CHAR_ARROW_RIGHT);
    }
}

void draw_critical_page(page_manager_t *pm, NHD_US2066_OLED *oled);
void draw_regen_page(page_manager_t *pm, NHD_US2066_OLED *oled);
void draw_traction_page(page_manager_t *pm, NHD_US2066_OLED *oled);
void draw_wheel_speed_page(page_manager_t *pm, NHD_US2066_OLED *oled);
void draw_fault_page(page_manager_t *pm, NHD_US2066_OLED *oled);

void page_manager_update(page_manager_t *pm, NHD_US2066_OLED *oled) {
    switch (pm->page) {
        case DASH_PAGE_CRITICAL:
            draw_critical_page(pm, oled);
            break;
        // case DASH_PAGE_CHARGE:
            // draw_charging_page(pm, oled);
            // break;
        case DASH_PAGE_REGEN:
            draw_regen_page(pm, oled);
            break;
        case DASH_PAGE_LIM_SELECT:
            draw_limit_select_page(pm, oled);
            break;
        case DASH_PAGE_TEMP_LIM:
            draw_temp_lim_page(pm, oled);
            break;
        case DASH_PAGE_VOLT_LIM:
            draw_volt_lim_page(pm, oled);
            break;
        case DASH_PAGE_TRACTION:
            draw_traction_page(pm, oled);
            break;
        case DASH_PAGE_MANUAL_LIMP:
          draw_manual_limp_page(pm, oled);
            break;
        //case DASH_PAGE_WHEEL_SPEED:
        //    draw_wheel_speed_page(pm, oled);
        //    break;
        case DASH_PAGE_FAULT:
            draw_fault_page(pm, oled);
            break;
        default:
            break;
    }
}

// looks like:

// R:HRTBEAT[PRECHARGE]
// TRQ 150    PACK 300V
// PWR 60kW   CELL 3.3V
// RPM 3400   TEMP  30C

void draw_critical_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_clearline(oled, 0);
    oled_set_pos(oled, 0, 0);

    carstats_t *stats = pm->stats;

    if (pm->stats->error_state == can0_VCUHeartbeat_error_state_RECOVERABLE_ERROR_STATE) {
        oled_print(oled, "R");
    } else if (pm->stats->error_state == can0_VCUHeartbeat_error_state_FATAL_ERROR_STATE) {
        oled_print(oled, "F");
    }

#define VCU_HEARTBEAT_TIMEOUT 1000 // ms

    if (msTicks > pm->stats->last_vcu_heartbeat + VCU_HEARTBEAT_TIMEOUT) {
        oled_rprint(oled, "\xFAVCU DEAD\xFC");
    } else {
        switch (pm->stats->vcu_state) {
            case can0_VCUHeartbeat_vcu_state_VCU_STATE_ROOT:
                oled_rprint(oled, "\xFAROOT\xFC");
                break;
            case can0_VCUHeartbeat_vcu_state_VCU_STATE_LV:
                oled_rprint(oled, "\xFALV\xFC");
                break;
            case can0_VCUHeartbeat_vcu_state_VCU_STATE_PRECHARGING:
                oled_rprint(oled, "\xFAPRECHARGE\xFC");
                break;
            case can0_VCUHeartbeat_vcu_state_VCU_STATE_RTD:
                oled_rprint(oled, "\xFARTD\xFC");
                break;
            case can0_VCUHeartbeat_vcu_state_VCU_STATE_DRIVING:
                oled_rprint(oled, "\xFA""DRIVE\xFC");
                break;
        }
    }

    oled_set_pos(oled, 1, 0);
    oled_clearline(oled, 1);
    oled_print(oled, "TRQ ");
    if (pm->stats->torque_mc >= 0) {
        int torque_Nm = pm->stats->torque_mc / 10;
        oled_print_num(oled, torque_Nm);
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }

    oled_rprint_pad(oled, "BUS", 6);
    if (pm->stats->DESIRED_VOLTAGE != -10) {
        int voltage = pm->stats->DESIRED_VOLTAGE / 10;
        oled_rprint_num_pad(oled, voltage, 1);
        oled_rprint(oled, "V");
    } else {
        oled_rprint(oled, DATA_UNKNOWN);
    }

    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);

    /*
    oled_print(oled, "PWR ");
    if (pm->stats->power >= 0) {
        int power_kW = pm->stats->power / 1000;
        oled_print_num(oled, power_kW);
        oled_print(oled, "kW");
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }*/
    oled_print(oled, "IGBT ");
    if (pm->stats->max_igbt_temp >= 0) {
        int igbt_temp_C = pm->stats->max_igbt_temp / 10;
        oled_print_num(oled, igbt_temp_C);
        oled_print(oled, "C");
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }

    oled_rprint_pad(oled, "CELL ", 4);
    if (pm->stats->min_cell_voltage >= 0) {
        int cell_mV = pm->stats->min_cell_voltage;
        oled_print_num_dec(oled, pm->stats->min_cell_voltage, 1000, 2);
    } else {
        oled_rprint(oled, DATA_UNKNOWN);
    }


    oled_clearline(oled, 3);
    oled_set_pos(oled, 3, 0);
    /*
    oled_print(oled, "RPM ");
    if (pm->stats->motor_rpm >= 0) {
        oled_print_num(oled, pm->stats->motor_rpm);
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }
    */
    oled_print(oled, "CUR ");
    if (pm->stats->cs_current != -10) {
        oled_print_num_dec(oled, pm->stats->cs_current, 1000, 2);
        oled_print(oled, "A");
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }

    oled_rprint_pad(oled, "TEMP ", 4);
    if (pm->stats->max_cell_temp >= 0) {
        oled_print_num_dec(oled, pm->stats->max_cell_temp, 10, 1);
    } else {
        oled_rprint(oled, DATA_UNKNOWN);
    }
}


void draw_regen_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    carstats_t *stats = pm->stats;

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);

    if (stats->buttons.right.rising_edge) {
        stats->controls.regen_bias += 1;
    }

    stats->controls.regen_bias = LOOPOVER(stats->controls.regen_bias, 25, 75);

    if (stats->buttons.B.rising_edge) stats->controls.using_regen ^= 1;  // NOT

    oled_print(oled, "REGEN ");

    oled_print(oled, (stats->controls.using_regen) ? "ON" : "OFF");

    oled_rprint_pad(oled, "BIAS ", 4);

    if (stats->controls.regen_bias != -1) {
        oled_print_num(oled, stats->controls.regen_bias);
    }
    else {
        oled_print(oled, DATA_UNKNOWN);
    }
}

void draw_limit_select_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    carstats_t *stats = pm->stats;

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);

    if (stats->buttons.right.rising_edge) stats->controls.using_voltage_limiting ^= 1; // NOT

    if (stats->buttons.B.rising_edge) stats->controls.using_temp_limiting ^= 1;

    oled_print(oled, "TEMP LIM: ");

    oled_print(oled, (stats->controls.using_temp_limiting) ? "ON" : "OFF");

    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);

    oled_print(oled, "VOLT LIM: ");

    oled_print(oled, (stats->controls.using_voltage_limiting) ? "ON" : "OFF");
}

void draw_temp_lim_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    carstats_t *stats = pm->stats;

    // Default should be 55
    if (stats->buttons.B.rising_edge) {
        stats->controls.temp_lim_min_gain += 1;
    }
    if (stats->buttons.right.rising_edge) {
        stats->controls.temp_lim_thresh_temp += 1;
    }

    stats->controls.temp_lim_min_gain = LOOPOVER(stats->controls.temp_lim_min_gain, 25, 50);
    stats->controls.temp_lim_thresh_temp = LOOPOVER(stats->controls.temp_lim_thresh_temp, 45, 60);

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);
    oled_print(oled, "TL MIN GAIN: ");
    if (stats->controls.temp_lim_min_gain != -1) {
        oled_print_num_dec(oled, stats->controls.temp_lim_min_gain, 100, 2);
    }
    else {
        oled_print(oled, DATA_UNKNOWN);
    }


    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);
    oled_print(oled, "THRESH TEMP: ");
    if (stats->controls.temp_lim_thresh_temp != -1) {
        oled_print_num(oled, stats->controls.temp_lim_thresh_temp);
        oled_print(oled, " C");
    }
    else {
        oled_print(oled, DATA_UNKNOWN);
    }
}

void draw_volt_lim_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    carstats_t *stats = pm->stats;

    if (stats->buttons.B.rising_edge) {
        stats->controls.volt_lim_min_gain += 1;
    }
    if (stats->buttons.right.rising_edge) {
        stats->controls.volt_lim_min_voltage += 5;
    }

    stats->controls.volt_lim_min_gain = LOOPOVER(stats->controls.volt_lim_min_gain, 25, 50);
    stats->controls.volt_lim_min_voltage = LOOPOVER(stats->controls.volt_lim_min_voltage, 250, 325);

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);
    oled_print(oled, "VL MIN GAIN: ");
    if (stats->controls.volt_lim_min_gain != -1) {
        oled_print_num_dec(oled, stats->controls.volt_lim_min_gain, 100, 2);
    }
    else {
        oled_print(oled, DATA_UNKNOWN);
    }


    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);
    oled_print(oled, "THRESH VOLTAGE: ");
    if (stats->controls.volt_lim_min_voltage != -1) {
        oled_print_num_dec(oled, stats->controls.volt_lim_min_voltage, 100, 2);
        oled_print(oled, " V");
    }
    else {
        oled_print(oled, DATA_UNKNOWN);
    }
}

void draw_manual_limp_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    carstats_t *stats = pm->stats;

    if (stats->buttons.B.rising_edge) {
        stats->controls.limp_factor += 10;
    }

    stats->controls.volt_lim_min_gain = LOOPOVER(stats->controls.volt_lim_min_gain, 25, 100);

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);
    oled_print(oled, "MANUAL LIMP: ");
    if (stats->controls.limp_factor != -1) {
        oled_print_num_dec(oled, stats->controls.limp_factor, 100, 1);
    }
    else {
        oled_print(oled, DATA_UNKNOWN);
    }

    stats->controls.limp_factor = LOOPOVER(stats->controls.limp_factor, 10, 100);
}

void draw_fault_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
  carstats_t *stats = pm->stats;

#define MAX_FAULTS 10

    char *fatal_faults[MAX_FAULTS] = {NULL};
    int n_fatal_faults = 0;
    char *recov_faults[MAX_FAULTS] = {NULL};
    int n_recov_faults = 0;
    char *gates[MAX_FAULTS] = {NULL};
    int n_gates = 0;

    can0_VCUErrors_T *errs = &stats->vcu_errors;

    // fatals
    if (errs->fatal_gate)
        fatal_faults[n_fatal_faults++] = "GATE";
    if (errs->fatal_contactor)
        fatal_faults[n_fatal_faults++] = "CONTACT";
    if (errs->fatal_precharge)
        fatal_faults[n_fatal_faults++] = "PRECHRG";
    if (errs->fatal_conflict)
        fatal_faults[n_fatal_faults++] = "ACC_IMP";

    // recoverables
    if (errs->recoverable_conflict)
        recov_faults[n_recov_faults++] = "BRK_IMP";
    if (errs->recoverable_gate)
        recov_faults[n_recov_faults++] = "GATE";
    if (errs->recoverable_contactor)
        recov_faults[n_recov_faults++] = "TSMS";
    if (errs->recoverable_heartbeat)
        recov_faults[n_recov_faults++] = "HRTBEAT";

    // gates
    if (errs->gate_sdn)
        gates[n_gates++] = "SDN";
    if (errs->gate_bpd)
        gates[n_gates++] = "BPD";
    if (errs->gate_bms)
        gates[n_gates++] = "BMS";
    if (errs->gate_imd)
        gates[n_gates++] = "IMD";


    // unnamed error?
    if (n_recov_faults == 0 && stats->error_state == can0_VCUHeartbeat_error_state_RECOVERABLE_ERROR_STATE) {
        recov_faults[n_recov_faults++] = "?";
    }
    if (n_fatal_faults == 0 && stats->error_state == can0_VCUHeartbeat_error_state_FATAL_ERROR_STATE) {
        fatal_faults[n_fatal_faults++] = "?";
    }

    oled_clearline(oled, 0);
    oled_clearline(oled, 1);
    oled_clearline(oled, 2);
    oled_clearline(oled, 3);

    oled_set_pos(oled, 0, 0);
    oled_print(oled, "FATAL: ");
    if (n_fatal_faults > 0) {
        for (int i = 0; i < n_fatal_faults; i++) {
            oled_print_wrap(oled, fatal_faults[i]);
            if (i < n_fatal_faults - 1)
                oled_print_wrap(oled, " ");
        }
    } else {
        oled_print(oled, "NONE :)");
    }

    // so many faults we ran out of space
    if (oled->line == 3) return;

    oled_set_pos(oled, oled->line + 1, 0);
    oled_print(oled, "RECOV: ");
    if (n_recov_faults > 0) {
        for (int i = 0; i < n_recov_faults; i++) {
            oled_print_wrap(oled, recov_faults[i]);
            if (i < n_recov_faults - 1)
                oled_print_wrap(oled, " ");
        }
    } else {
        oled_print(oled, "NONE :)");
    }

    // so many faults we ran out of space
    if (oled->line == 3) return;

    if (n_gates > 0) {
        oled_set_pos(oled, oled->line + 1, 0);
        oled_print(oled, "GATE:");

        for (int i = 0; i < n_gates; i++) {
            oled_print_wrap(oled, gates[i]);
            if (i < n_gates - 1)
                oled_print(oled, " ");
        }
    }
}

void draw_charging_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    carstats_t *stats = pm->stats;

    oled_clearline(oled, 0);
    oled_set_pos(oled, 0, 0);
    oled_print(oled, "CHARGING");

    oled_rprint_pad(oled, "SOC", 4);
    oled_rprint_num(oled, stats->soc);

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);
    oled_print(oled, "BUS ");
    oled_print_num(oled, stats->cs_voltage / 10);
    oled_print(oled, "V");
    oled_set_pos(oled, 1, 8);
    oled_print_num_dec(oled, pm->stats->cs_current, 1000, 2);
    oled_print(oled, "A");

    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);
    oled_print(oled, "CELL ");
    oled_print_num(oled, stats->min_cell_voltage/1000);
    oled_print(oled, "V / ");
    oled_print_num(oled, stats->max_cell_voltage/1000);
    oled_print(oled, "V");

    oled_clearline(oled, 3);
    oled_set_pos(oled, 3, 0);
    oled_print(oled, "TEMP ");
    oled_print_num(oled, stats->min_cell_temp/10);
    oled_print(oled, "C / ");
    oled_print_num(oled, stats->max_cell_temp/10);
    oled_print(oled, "C");
}

inline uint16_t convert_adc_to_psi(uint16_t adc) {
  return (2019 * adc) / 1000 - 188;
}

void draw_traction_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    int brake_1 = convert_adc_to_psi(pm->stats->brake_1);
    int brake_2 = convert_adc_to_psi(pm->stats->brake_2);

    oled_set_pos(oled, 1, 0);
    oled_clearline(oled, 1);
    oled_print(oled, "RA ");
    if (pm->stats->brake_1 >= 0) {
        uint16_t rat = (100 * brake_1) / (brake_1 + brake_2);
        oled_print_num_dec(oled, rat, 100, 2);
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }

    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);

    oled_print(oled, "B1 ");
    if (pm->stats->brake_1 >= 0) {
        oled_print_num(oled, brake_1);
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }

    oled_clearline(oled, 3);
    oled_set_pos(oled, 3, 0);

    oled_print(oled, "B2 ");
    if (pm->stats->brake_2 >= 0) {
        oled_print_num(oled, brake_2);
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }
}

// looks like:

//
//
// 1000RPM      1000RPM
// 1000RPM      1000RPM
void draw_wheel_speed_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);
    oled_print_num(oled, pm->stats->front_left_wheel_speed);
    oled_print(oled, "RPM");
    oled_rprint_num_pad(oled, pm->stats->front_right_wheel_speed, 3);
    oled_rprint(oled, "RPM");

    oled_clearline(oled, 3);
    oled_set_pos(oled, 3, 0);
    oled_print_num(oled, pm->stats->rear_left_wheel_speed);
    oled_print(oled, "RPM");
    oled_rprint_num_pad(oled, pm->stats->rear_right_wheel_speed, 3);
    oled_rprint(oled, "RPM");
}
