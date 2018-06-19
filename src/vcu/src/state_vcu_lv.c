#include "state_vcu_lv.h"

static bool   latch_driver_rst = false;
static Time_T driver_rst_timer;

const Time_T DRIVER_RST_LATCH_TIME = 50;

void enter_vcu_state_lv(void) {
  printf("[VCU FSM : LV] ENTERED!\r\n");
  printf("[VCU FSM : LV] Currently waiting for DRIVER RESET press.\r\n");

  latch_driver_rst = false;
  driver_rst_timer = HAL_GetTick();
}

void update_vcu_state_lv(void) {
  if (latch_driver_rst) {
    if (HAL_GetTick() - driver_rst_timer > DRIVER_RST_LATCH_TIME) {
      HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_SET);
      printf("[VCU FSM : LV] Setting DRIVER_RST pin HIGH again.\r\n");

      set_vcu_state(VCU_STATE_PRECHARGING);
      return;
    }
  }
  else if (buttons.DriverReset) {
    printf("[VCU FSM : LV] DRIVER RESET pressed.\r\n");
    printf("[VCU FSM : LV] Setting DRIVER_RST pin LOW for %d ms...\r\n",
           DRIVER_RST_LATCH_TIME);

    HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_RESET);

    latch_driver_rst = true;
    driver_rst_timer = HAL_GetTick();
  }
}
