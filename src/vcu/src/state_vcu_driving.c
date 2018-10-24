#include "state_vcu_driving.h"

const Time_T print_period = 20;

static const Time_T RTD_HOLD = 200;

static bool   rtd_started;
static Time_T rtd_last;

static bool stall_until_safe = true;

void enter_vcu_state_driving(void) {
  printf("[VCU FSM : DRIVING] ENTERED!\r\n");

  stall_until_safe = true;
  printf("[VCU FSM : DRIVING] Release accelerator and RTD.\r\n");

  rtd_started = false;
  rtd_last    = 0;

  printf("CONTROLS PARAMS:\r\n  using_regen: %d\r\n  using_voltage_limiting: %d\r\n  using_temp_limiting: %d\r\n  regen_bias: %d\r\n  limp_factor: %d\r\n  temp_lim_min_gain: %d\r\n  temp_lim_thresh_temp: %d\r\n  temp_abs_thesh: %d\r\n  volt_lim_min_gain: %d\r\n  volt_lim_min_voltage: %d\r\n  volt_abs_thresh: %d\r\n\r\n",
   control_settings.using_regen, control_settings.using_voltage_limiting, control_settings.using_temp_limiting, control_settings.regen_bias, control_settings.limp_factor, control_settings.temp_lim_min_gain, control_settings.temp_lim_thresh_temp, MAX_TEMP, control_settings.volt_lim_min_gain, control_settings.volt_lim_min_voltage, MIN_VOLTAGE);
  enable_controls();
}

void update_vcu_state_driving(void) {
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

  // Arguably should be in controls, but it's not going to be autonomous this year.
  HAL_GPIO_WritePin(GPIO(ACTIVE_AERO), control_settings.active_aero_enabled);

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
