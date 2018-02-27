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
    draw_nav_line(pm, oled);

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

void draw_critical_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_clearline(oled, 0);
    oled_set_pos(oled, 0, 0);
    oled_print(oled, "PWR  50kW");
    oled_rprint(oled, "PACK 300V");

    oled_clearline(oled, 1);
    oled_set_pos(oled, 1, 0);
    oled_print(oled, "TEMP 30C");
    oled_rprint(oled, "CELL 3.3V");
}

void draw_takeover_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    //oled_clearline(oled, 0);
}

void draw_traction_page(page_manager_t *pm, NHD_US2066_OLED *oled) {
    oled_clearline(oled, 2);
    oled_set_pos(oled, 2, 0);
    oled_print(oled, "TORQUE ");
    oled_print(oled, pm->stats->torque_requested);
    oled_print(oled, "/");
    oled_print(oled, pm->stats->torque_mc);
}

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
