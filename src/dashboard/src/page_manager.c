#include "page_manager.h"

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
}

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
        case DASH_PAGE_TAKEOVER:
            draw_takeover_page(pm, oled);
            break;
        case DASH_PAGE_TRACTION:
            draw_traction_page(pm, oled);
            break;
        case DASH_PAGE_WHEEL_SPEED:
            draw_wheel_speed_page(pm, oled);
            break;
        default:
            break;
    }
}

// looks like:

// [AERO]   [PRECHARGE]
// TRQ 150    PACK 300V
// PWR 60kW   CELL 3.3V
// RPM 3400   TEMP  30C

void draw_critical_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_set_pos(oled, 0, 1);
    if (pm->stats->vcu_data.active_aero_on && false) {
        oled_print_char(oled, CHAR_LEFT_BRACKET);
        oled_print(oled, "AERO");
        oled_print_char(oled, CHAR_RIGHT_BRACKET);
    }

    
    switch (pm->stats->vcu_state) {
        case VCU_STATE_ROOT:
        case VCU_STATE_LV:
            oled_rprint(oled, "\xFALV\xFC");
            break;
        case VCU_STATE_PRECHARGING:
            oled_rprint(oled, "\xFAPRECHARGE\xFC");
            break;
        case VCU_STATE_RTD:
            oled_rprint(oled, "\xFARTD\xFC");
            break;
        case VCU_STATE_DRIVING:
            oled_rprint(oled, "\xFADRIVE\xFC");
            break;
    }

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);
    oled_print(oled, "TRQ ");
    int torque_Nm = pm->stats->torque_mc / 10;
    oled_print_num(oled, torque_Nm);

    int pack_V = pm->stats->battery_voltage;
    oled_rprint_pad(oled, "PACK", 5);
    oled_rprint_num_pad(oled, pack_V, 1);
    oled_rprint(oled, "V");

    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);

    int power_kW = pm->stats->power / 1000;
    oled_print(oled, "PWR ");
    oled_print_num(oled, power_kW);
    oled_print(oled, "kW");

    int cell_mV = pm->stats->lowest_cell_voltage;
    int cell_V = cell_mV / 1000;
    int cell_dV = (cell_mV / 100) % 10;
    oled_rprint_pad(oled, "CELL", 5);
    oled_rprint_num_pad(oled, cell_V, 3);
    oled_rprint_pad(oled, ".", 2);
    oled_rprint_num_pad(oled, cell_dV, 1);
    oled_rprint(oled, "V");

    
    oled_clearline(oled, 3);
    oled_set_pos(oled, 3, 0);
    oled_print(oled, "RPM ");
    oled_print_num(oled, pm->stats->motor_rpm);

    int temp_C = pm->stats->max_cell_temp / 10;
    oled_rprint_pad(oled, "TEMP", 5);
    oled_rprint_num_pad(oled, temp_C, 1);
    oled_rprint(oled, "C");
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
