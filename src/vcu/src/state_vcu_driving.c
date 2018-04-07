#include "state_vcu_driving.h"

const Time_T print_period = 1000;

static uint16_t torque_command   = 0;
static bool     stall_until_safe = true;

void resetDrivingValues() {
  torque_command = 0;
}

void enter_vcu_state_driving() {
  printf("[VCU FSM : DRIVING] ENTERED!\r\n");
  resetDrivingValues();

  stall_until_safe = true;
  printf("[VCU FSM : DRIVING] Release accelerator and RTD.\r\n");
}

void update_vcu_state_driving() {
  if (stall_until_safe) {
    // Do not drive yet unless everything has been let go.
    if (buttons.RTD || pedalbox_min(accel) > PEDALBOX_ACCEL_RELEASE) return;

    printf("[VCU FSM : DRIVING] You may drive, heathen. GLHF\r\n");
    stall_until_safe = false;
  }

  // Send torque commands
  // torque_command = calcTorque(pedalbox_avg(accel));
  torque_command = 0;

  sendTorqueCmdMsg(torque_command);

  static Time_T last_print = 0;

  if (HAL_GetTick() - last_print > print_period) {
    printf("A1:%d A2:%d B1:%d B2:%d T:%d\r\n",
           pedalbox.accel_1,
           pedalbox.accel_2,
           pedalbox.brake_1,
           pedalbox.brake_2,
           torque_command);

    last_print = HAL_GetTick();
  }

  if (buttons.RTD) {
    printf("[VCU FSM : DRIVING] RTD pressed. Going to RTD mode. Makes sense amirite?\r\n");
    set_vcu_state(VCU_STATE_RTD);
    return;
  }
}
