#include "dispatch.h"

#include "board.h"
#include "button_listener.h"
#include "NHD_US2066.h"

static button_state_t left_button;
static button_state_t right_button;

static NHD_US2066_OLED oled;

static page;

static uint32_t nextOLEDUpdate;

#define BUTTON_DOWN false

void dispatch_init() {
    init_button_state(&left_button);
    init_button_state(&right_button);

    oled_init(&oled, NHD_0420CW_NLINES, NHD_0420CW_NCOLS);
    oled_init_commands(&oled);
    oled_clear(&oled);
    Delay(100);

    page = 0; 

    nextOLEDUpdate = 0;
}

void print_page0();
void print_page1();
void print_page2();
void print_page3();
void print_page4();

void dispatch_update() {
    bool left_button_down  = (Pin_Read(PIN_BUTTON1) == BUTTON_DOWN);
    bool right_button_down = (Pin_Read(PIN_BUTTON2) == BUTTON_DOWN);
    update_button_state(&left_button, left_button_down);
    update_button_state(&right_button, right_button_down);

    if (right_button.action == BUTTON_ACTION_TAP) {
        page++;
        if (page > 4) page = 4;
        oled_clear(&oled);
    } else if (right_button.action == BUTTON_ACTION_HOLD) {
        page--;
        if (page < 0) page = 0;
        oled_clear(&oled);
    }

    if (msTicks > nextOLEDUpdate) {
        nextOLEDUpdate = msTicks + 150;

        switch (page) {
            case 0:
                print_page0();
                break;
            case 1:
                print_page1();
                break;
            case 2:
                print_page2();
                break;
            case 3:
                print_page3();
                break;
            case 4:
                print_page4();
            default:
                break;
        }

        oled_update(&oled);
    }
}

void print_page0() {

    oled_set_pos(&oled, 2, 0);
    oled_print(&oled, "500RPM");
    oled_rprint(&oled, "500RPM");
    oled_set_pos(&oled, 3, 0);
    oled_print(&oled, "500RPM");
    oled_rprint(&oled, "500RPM");

    oled_set_pos(&oled, 0, 0);
    oled_rprint_pad(&oled, "TRACTION", 1);
    oled_print_char(&oled, CHAR_ARROW_RIGHT);
}
void print_page1() {
    oled_set_pos(&oled, 0, 0);
    oled_print_char(&oled, CHAR_ARROW_LEFT);
    oled_print(&oled, "SPEED");

    oled_rprint_pad(&oled, "CRITICAL", 1);
    oled_print_char(&oled, CHAR_ARROW_RIGHT);

    oled_set_pos(&oled, 2, 5);
    oled_print(&oled, "TORQUE: 50");
}
void print_page2() {
    oled_set_pos(&oled, 0, 0);
    oled_print_char(&oled, CHAR_ARROW_LEFT);
    oled_print(&oled, "TRACTION");

    oled_rprint_pad(&oled, "TAKEOVER", 1);
    oled_print_char(&oled, CHAR_ARROW_RIGHT);

    oled_set_pos(&oled, 2, 0);
    oled_print(&oled, "POWER 60kW");
    oled_rprint(&oled, "TEMP 35C");

    oled_set_pos(&oled, 3, 0);
    oled_print(&oled, "MIN 2.9V");
    oled_rprint(&oled, "SOC 86");

}
void print_page3() {

}
void print_page4() {

}
