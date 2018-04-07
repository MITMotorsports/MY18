#include "state_vcu_rtd.h"

const Time_T RTD_HOLD = 1500;

static bool   rtd_started;
static Time_T rtd_last;

void enter_vcu_state_rtd() {
  printf("[VCU FSM : RTD] ENTERED!\r\n");
  printf("[VCU FSM : RTD] Hold RTD button for %dms and press brake.\r\n", RTD_HOLD);
  rtd_started = false;
  rtd_last    = 0;
}

void update_vcu_state_rtd() {
  bool brk_pressed = pedalbox_max(brake) > PEDALBOX_BRAKE_BEGIN;

  static Time_T last_print = 0;

  if (HAL_GetTick() - last_print > 1000) {
    printf("[RTD] A1:%d A2:%d B1:%d B2:%d\r\n",
           pedalbox.accel_1,
           pedalbox.accel_2,
           pedalbox.brake_1,
           pedalbox.brake_2);

    last_print = HAL_GetTick();
  }

  if (buttons.RTD) {
    if (rtd_started) {
      if (HAL_GetTick() - rtd_last > RTD_HOLD) {
        if (brk_pressed) {
          set_vcu_state(VCU_STATE_DRIVING);
          return;
        }
      }
    }
    else {
      printf("[VCU : RTD] RTD pressed. Keep holdin'.\r\n");
      rtd_last = HAL_GetTick();
    }
  }

  rtd_started = buttons.RTD;
}
