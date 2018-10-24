#include "dispatch.h"

#include "board.h"
#include "button_listener.h"
#include "page_manager.h"
#include "carstats.h"
#include "NHD_US2066.h"
#include "led.h"

static button_state_t left_button;
static button_state_t right_button;

static page_manager_t page_manager;
static carstats_t carstats = {};

static NHD_US2066_OLED oled;

static uint32_t nextOLEDUpdate;

static bool active_aero_enabled = false;

#define BUTTON_DOWN false
#define OLED_UPDATE_INTERVAL_MS 50

// dead after (ms)
#define BMS_HEARTBEAT_EXPIRE 1000

void update_lights(void);
void send_dash_controls(void);

void dispatch_init() {
    init_button_state(&left_button);
    init_button_state(&right_button);

    page_manager_init(&page_manager, &carstats);

    oled_init(&oled);
    Delay(50);
    oled_init_commands(&oled);
    Delay(50);
    oled_clear(&oled);
    Delay(50);

    // necessary
    oled_set_double_height_mode(&oled, NHD_US2066_DOUBLE_HEIGHT_MIDDLE);
    oled_set_pos(&oled, 0, 0);
    oled_print_char(&oled, CHAR_DIAMOND);
    for (int i = 1; i < 19; i++) oled_print(&oled, "-");
    oled_print_char(&oled, CHAR_DIAMOND);
    oled_set_pos(&oled, 1, 0);
    oled_print_char(&oled, CHAR_PIPE);
    oled_set_pos(&oled, 1, 8);
    oled_print(&oled, "MY18");
    oled_set_pos(&oled, 1, 19);
    oled_print_char(&oled, CHAR_PIPE);
    oled_set_pos(&oled, 2, 0);
    oled_print_char(&oled, CHAR_DIAMOND);
    for (int i = 1; i < 19; i++) oled_print(&oled, "-");
    oled_print_char(&oled, CHAR_DIAMOND);
    oled_update(&oled);
    Delay(600);
    oled_set_double_height_mode(&oled, NHD_US2066_DOUBLE_HEIGHT_NONE);
    oled_clear(&oled);


    nextOLEDUpdate = 0;

    // init carstats fields
    carstats.mc_voltage              = -10;  // TODO: Make less sketchy for unknown values.
    carstats.cs_voltage              = -10;
    carstats.cs_current              = -10;
    carstats.min_cell_voltage        = -1;
    carstats.max_cell_voltage        = -1;
    carstats.min_cell_temp           = -1;
    carstats.max_cell_temp           = -1;
    carstats.power                   = -1;
    carstats.soc                     = -1;
    carstats.torque_mc               = -1;
    carstats.motor_rpm               = -1;
    carstats.front_left_wheel_speed  = -1;
    carstats.front_right_wheel_speed = -1;
    carstats.rear_left_wheel_speed   = -1;
    carstats.rear_right_wheel_speed  = -1;
    carstats.max_igbt_temp           = -1;
    carstats.brake_1                 = -1;
    carstats.brake_2                 = -1;
    carstats.vcu_state               = can0_VCUHeartbeat_vcu_state_VCU_STATE_ROOT;
    carstats.last_vcu_heartbeat      = msTicks;
    carstats.last_bms_heartbeat      = msTicks;

    carstats.controls.regen_bias             = -1;
    carstats.controls.limp_factor            = -1;
    carstats.controls.temp_lim_min_gain      = 25;
    carstats.controls.temp_lim_thresh_temp   = 50;
    carstats.controls.volt_lim_min_gain      = 0;
    carstats.controls.volt_lim_min_voltage   = 250;
    carstats.controls.using_regen            = false;
    carstats.controls.using_temp_limiting    = false;
    carstats.controls.using_voltage_limiting = false;
    carstats.controls.active_aero_enabled    = false;

    carstats.vcu_controls_received = false;

    init_button_state(&carstats.buttons.left);
    init_button_state(&carstats.buttons.right);
    init_button_state(&carstats.buttons.A);
    init_button_state(&carstats.buttons.B);
}

inline void dispatch_update() {
    update_button_state(&carstats.buttons.left,  (Pin_Read(PIN_BUTTON_LEFT) == BUTTON_DOWN));
    update_button_state(&carstats.buttons.right, (Pin_Read(PIN_BUTTON_RIGHT) == BUTTON_DOWN));
    update_button_state(&carstats.buttons.A, carstats.button_bank.A);
    update_button_state(&carstats.buttons.B, carstats.button_bank.B);

    can_update_carstats(&carstats);
    vcu_controls_update();

    // switch (carstats.buttons.A.action) {
    //   case BUTTON_ACTION_TAP:
    //     page_manager_next_page(&page_manager);
    //     oled_clear(&oled);
    //     break;
    //   case BUTTON_ACTION_HOLD:
    //     page_manager.page  = DASH_PAGE_CRITICAL;
    // }

    if (carstats.buttons.A.rising_edge) {
        page_manager_next_page(&page_manager);
        oled_clear(&oled);
    }

    update_lights();

    // Run the update always, such that the carstats set in the update also change.
    page_manager_update(&page_manager, &oled);
    if (msTicks > nextOLEDUpdate) {
        nextOLEDUpdate = msTicks + OLED_UPDATE_INTERVAL_MS;
        oled_update(&oled);
    }

    send_dash_controls();
}

void update_lights(void) {
    // TODO: RTD, IMD, AMS lights
    if (carstats.vcu_state == can0_VCUHeartbeat_vcu_state_VCU_STATE_DRIVING)
        LED_RTD_on();
    else
        LED_RTD_off();

    if (carstats.mc_voltage / 10 > 60)
        LED_HV_on();
    else
        LED_HV_off();

    if (carstats.vcu_errors.gate_imd)
        LED_IMD_on();
    else
        LED_IMD_off();

    if (carstats.vcu_errors.gate_bms)
        LED_AMS_on();
    else
        LED_AMS_off();

}

void send_dash_controls(void) {
    LIMIT(can0_DashRequest_period);
    handle_can_error(can0_DashRequest_Write(&carstats.controls));
}

void vcu_controls_update(void) {
    if (carstats.vcu_controls_received) {
        if (carstats.controls.regen_bias == 255) {
            carstats.controls.regen_bias = carstats.vcu_controls.regen_bias;
            carstats.controls.using_regen = carstats.vcu_controls.using_regen;
        }

        if (carstats.controls.limp_factor == 255) {
          carstats.controls.limp_factor = carstats.vcu_controls.limp_factor;
        }

        if (carstats.controls.temp_lim_min_gain == 255) {
          carstats.controls.temp_lim_min_gain = carstats.vcu_controls.temp_lim_min_gain;
          carstats.controls.using_temp_limiting = carstats.vcu_controls.using_temp_limiting;
        }
        if (carstats.controls.temp_lim_thresh_temp == 255) {
          carstats.controls.temp_lim_thresh_temp = carstats.vcu_controls.temp_lim_thresh_temp;
          carstats.controls.using_temp_limiting = carstats.vcu_controls.using_temp_limiting;
        }

        if (carstats.controls.volt_lim_min_gain == 255) {
          carstats.controls.volt_lim_min_gain = carstats.vcu_controls.volt_lim_min_gain;
        }
        if (carstats.controls.volt_lim_min_voltage == 65535) {
          carstats.controls.volt_lim_min_voltage = carstats.vcu_controls.volt_lim_min_voltage;
        }

        if (carstats.vcu_controls.torque_temp_limited) {
          carstats.controls.using_regen = false;
        }
    }

}
