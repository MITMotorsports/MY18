#include "state_vcu_driving.h"

const Time_T print_period = 20;

static const Time_T RTD_HOLD = 200;

static bool   rtd_started;
static Time_T rtd_last;

static bool stall_until_safe = true;

void enter_vcu_state_driving() {
  printf("[VCU FSM : DRIVING] ENTERED!\r\n");

  stall_until_safe = true;
  printf("[VCU FSM : DRIVING] Release accelerator and RTD.\r\n");

  rtd_started = false;
  rtd_last    = 0;

  printf("CONTROLS PARAMS:\r\n  USING REGEN: %d\r\n  cBB_ef: %d\r\n  LIMP FACTOR: %d\r\n", control_settings.using_regen, control_settings.cBB_ef, control_settings.limp_factor);

  enable_controls();
}

void update_vcu_state_driving() {
  if (stall_until_safe) {
    sendMotorOffCmdMsg();

    // Do not drive yet unless everything has been let go.
    if (buttons.RTD || (pedalbox_min(accel) > PEDALBOX_ACCEL_RELEASE)) return;

    printf("[VCU FSM : DRIVING] You may drive, heathen. GLHF\r\n");
    stall_until_safe = false;
  }

  // Calculate and send motor controller commands.
  if (!get_controls_enabled()) {
    enable_controls();
  }
  execute_controls();

  if (buttons.RTD) {
    if (rtd_started) {
      if (HAL_GetTick() - rtd_last > RTD_HOLD) {
        set_vcu_state(VCU_STATE_RTD);
        return;
      }
    }
    else {
      printf("[VCU : DRIVING] RTD pressed. Hold to disable.'.\r\n");
      rtd_last = HAL_GetTick();
    }
  }

  rtd_started = buttons.RTD;
}
