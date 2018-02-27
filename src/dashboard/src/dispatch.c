#include "dispatch.h"

#include "board.h"
#include "button_listener.h"
#include "page_manager.h"
#include "carstats.h"
#include "NHD_US2066.h"

static button_state_t left_button;
static button_state_t right_button;

static page_manager_t page_manager;
static carstats_t carstats;

static NHD_US2066_OLED oled;

static uint32_t nextOLEDUpdate;

#define BUTTON_DOWN false
#define OLED_UPDATE_INTERVAL_MS 30

void dispatch_init() {
    init_button_state(&left_button);
    init_button_state(&right_button);

    page_manager_init(&page_manager, &carstats);

    oled_init(&oled, NHD_0420CW_NLINES, NHD_0420CW_NCOLS);
    oled_init_commands(&oled);
    oled_clear(&oled);
    Delay(100);

    nextOLEDUpdate = 0;
}

void dispatch_update() {
    bool left_button_down  = (Pin_Read(PIN_BUTTON1) == BUTTON_DOWN);
    bool right_button_down = (Pin_Read(PIN_BUTTON2) == BUTTON_DOWN);
    update_button_state(&left_button, left_button_down);
    update_button_state(&right_button, right_button_down);

    if (right_button.action == BUTTON_ACTION_TAP) {
        page_manager_next_page(&page_manager);
    } else if (right_button.action == BUTTON_ACTION_HOLD) {
        page_manager_prev_page(&page_manager);
    }

    if (msTicks > nextOLEDUpdate) {
        nextOLEDUpdate = msTicks + OLED_UPDATE_INTERVAL_MS;
        page_manager_update(&page_manager, &oled);
        oled_update(&oled);
    }
}
