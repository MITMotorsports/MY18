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
#define OLED_UPDATE_INTERVAL_MS 100

void dispatch_init() {
    init_button_state(&left_button);
    init_button_state(&right_button);

    page_manager_init(&page_manager, &carstats);

    oled_init(&oled);
    Delay(100);
    oled_init_commands(&oled);
    Delay(100);
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

    can_update_carstats(&carstats);
    update_lights();

    if (msTicks > nextOLEDUpdate) {
        nextOLEDUpdate = msTicks + OLED_UPDATE_INTERVAL_MS;
        page_manager_update(&page_manager, &oled);
        oled_update(&oled);
    }
}

void update_lights() {
    if (carstats->vcu_data.rtd_light_on)
        LED_RTD_on();
    else
        LED_RTD_off();

    if (carstats->vcu_data.hv_light_on)
        LED_HV_on();
    else
        LED_HV_off();

    if (carstats->vcu_data.imd_light_on)
        LED_IMD_on();
    else
        LED_IMD_off();

    if (carstats->vcu_data.ams_light_on)
        LED_AMS_on();
    else
        LED_AMS_off();

}
