#include "page_manager.h"

#include "board.h"
#include "NHD_US2066_charset.h"

#define DATA_UNKNOWN "?"

void page_manager_init(page_manager_t *pm, carstats_t *stats) {
    pm->page = 0;
    pm->stats = stats;
}

void page_manager_next_page(page_manager_t *pm) {
    // wrap around
    pm->page = (pm->page + 1) % DASH_PAGE_COUNT;

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
        case DASH_PAGE_TAKEOVER:
            return "takeover";
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
void draw_takeover_page(page_manager_t *pm, NHD_US2066_OLED *oled);
void draw_traction_page(page_manager_t *pm, NHD_US2066_OLED *oled);
void draw_wheel_speed_page(page_manager_t *pm, NHD_US2066_OLED *oled);

void page_manager_update(page_manager_t *pm, NHD_US2066_OLED *oled) {
    switch (pm->page) {
        case DASH_PAGE_CRITICAL:
            draw_critical_page(pm, oled);
            break;
        case DASH_PAGE_CHARGE:
            draw_charging_page(pm, oled);
            break;
        //case DASH_PAGE_TAKEOVER:
        //    draw_takeover_page(pm, oled);
        //    break;
        //case DASH_PAGE_TRACTION:
        //    draw_traction_page(pm, oled);
        //    break;
        //case DASH_PAGE_WHEEL_SPEED:
        //    draw_wheel_speed_page(pm, oled);
        //    break;
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
    if (stats->vcu_errors.recoverable_conflict) {
        oled_print(oled, "R:CONFLCT");
    } else if (stats->vcu_errors.recoverable_gate) {
        oled_print(oled, "R:GATE");
    } else if (stats->vcu_errors.recoverable_heartbeat) {
        oled_print(oled, "R:HRTBEAT");
    } else if (stats->vcu_errors.recoverable_contactor) {
        oled_print(oled, "R:CONTACT");
    } else if (stats->vcu_errors.fatal_contactor) {
        oled_print(oled, "F:CONTACT");
    } else if (stats->vcu_errors.fatal_gate) {
        oled_print(oled, "F:GATE");
    } else if (stats->vcu_errors.fatal_precharge) {
        oled_print(oled, "F:PRECHRG");
    } else if (stats->vcu_errors.fatal_conflict) {
        oled_print(oled, "F:CONFLCT");
    } else {
        if (pm->stats->error_state == can0_VCUHeartbeat_error_state_RECOVERABLE_ERROR_STATE) {
            oled_print(oled, "RECOV");
        } else if (pm->stats->error_state == can0_VCUHeartbeat_error_state_FATAL_ERROR_STATE) {
            oled_print(oled, "FATAL");
        }
    }

    if (msTicks > pm->stats->last_vcu_heartbeat + 1000) {
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
    /*
    oled_print(oled, "TRQ ");
    if (pm->stats->torque_mc >= 0) {
        int torque_Nm = pm->stats->torque_mc / 10;
        oled_print_num(oled, torque_Nm);
    } else {
        oled_print(oled, DATA_UNKNOWN);
    }
    */
    oled_print(oled, "PWR ");
    //if (pm->stats->power >= 0) {
        int power = pm->stats->power;
        oled_print_num(oled, power);
        oled_print(oled, "W");
    //} else {
    //    oled_print(oled, DATA_UNKNOWN);
   // }

    oled_rprint_pad(oled, "BUS", 6);
    if (pm->stats->voltage_2 >= 0) {
        //int pack_V = pm->stats->battery_voltage/10;
        int pack_V = pm->stats->voltage_2/1000;
        oled_rprint_num_pad(oled, pack_V, 1);
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

    oled_rprint_pad(oled, "CELL", 5);
    if (pm->stats->min_cell_voltage >= 0) {
        int cell_mV = pm->stats->min_cell_voltage;
        int cell_V = cell_mV / 1000;
        int cell_dV = (cell_mV / 100) % 10;
        int cell_cV = (cell_mV / 10) % 10;
        oled_rprint_num_pad(oled, cell_V, 3);
        oled_rprint_pad(oled, ".", 2);
        oled_rprint_num_pad(oled, cell_dV, 1);
        oled_rprint_num(oled, cell_cV);
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
    //if (pm->stats->current >= 0) {
        //oled_print_num(oled, pm->stats->current / 1000);
        //oled_print_num(oled, ".");
        //oled_print_num(oled, (pm->stats->current / 100)%10);
        oled_print(oled, "A");
    //} else {
        //oled_print(oled, DATA_UNKNOWN);
    //}

    oled_rprint_pad(oled, "TEMP", 5);
    if (pm->stats->max_cell_temp >= 0) {
        int temp_C = pm->stats->max_cell_temp / 10;
        oled_rprint_num_pad(oled, temp_C, 1);
        oled_rprint(oled, "C");
    } else {
        oled_rprint(oled, DATA_UNKNOWN);
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
    oled_print_num(oled, stats->battery_voltage/10);
    oled_print(oled, "V");
    oled_set_pos(oled, 1, 8);
    oled_print_num(oled, stats->battery_current);
    oled_print(oled, "mA");

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

void draw_takeover_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    //oled_clearline(oled, 0);
}

void draw_traction_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);
    oled_print(oled, "TORQUE ");
    oled_print_num(oled, pm->stats->torque_mc);
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
