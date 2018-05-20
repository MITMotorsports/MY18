#include "state_vcu_lv.h"

const Time_T DRIVER_RST_LATCH_TIME = 50;

// Variables for holding the VCU driver reset pin HIGH to reset SDN_GATE.
static bool   latch_driver_rst = false;
static Time_T driver_rst_timer;

// Variables for establishing setup time for Button Bank's driver reset button.
static bool   driver_rst_hit_started;
static Time_T driver_rst_hit_last;
const Time_T DRIVER_RST_HIT_HOLD = 600;

void enter_vcu_state_lv() {
  printf("[VCU FSM : LV] ENTERED!\r\n");
  printf("[VCU FSM : LV] Currently waiting for DRIVER RESET press.\r\n");

  latch_driver_rst = false;
  driver_rst_timer = HAL_GetTick();

  driver_rst_hit_started = false;
  driver_rst_hit_last    = 0;
}

void update_vcu_state_lv() {
  if (latch_driver_rst) {
    if (HAL_GetTick() - driver_rst_timer > DRIVER_RST_LATCH_TIME) {
      HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_SET);
      printf("[VCU FSM : LV] Setting DRIVER_RST pin HIGH again.\r\n");

      set_vcu_state(VCU_STATE_PRECHARGING);
      return;
    }
  }
  else if (buttons.DriverReset) {
    if (driver_rst_hit_started) {
      if (HAL_GetTick() - driver_rst_hit_last > DRIVER_RST_HIT_HOLD) {
        printf("[VCU FSM : LV] Setting DRIVER_RST pin LOW for %d ms...\r\n",
               DRIVER_RST_LATCH_TIME);

        HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_RESET);

        latch_driver_rst = true;
        driver_rst_timer = HAL_GetTick();
      }
    }
    else {
      printf("[VCU FSM : LV] DRIVER RESET pressed. Keep holdin'.\r\n");
      driver_rst_hit_last = HAL_GetTick();
    }
  }

  driver_rst_hit_started = buttons.DriverReset;
}
