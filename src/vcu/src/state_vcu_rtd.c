#include "state_vcu_rtd.h"

const Time_T RTD_HOLD = 1000;

static bool   rtd_started;
static Time_T rtd_last;

void enter_vcu_state_rtd(void) {
  printf("[VCU FSM : RTD] ENTERED!\r\n");
  printf("[VCU FSM : RTD] Hold RTD button for %dms and press brake.\r\n", RTD_HOLD);
  rtd_started = false;
  rtd_last    = 0;

  // Clear MC faults
  printf("[VCU FSM : RTD] Clearing MC faults...\r\n");
  send_mc_fault_clear();
  sendMotorOffCmdMsg();

  disable_controls();

  // Turn on battery fans
  HAL_GPIO_WritePin(GPIO(BFAN), GPIO_PIN_SET);
}

void update_vcu_state_rtd(void) {
  // Keep the motor clear of faults
  sendMotorOffCmdMsg();

  bool brk_pressed = pedalbox_max(brake) > PEDALBOX_BRAKE_RTD;

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
