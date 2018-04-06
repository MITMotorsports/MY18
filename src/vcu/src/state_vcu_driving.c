#include "state_vcu_driving.h"

const Time_T print_period = 1000;

static uint16_t torque_command = 0;

void resetDrivingValues() {
  torque_command   = 0;
}

void enter_vcu_state_driving() {
  printf("[VCU FSM : DRIVING] ENTERED!\r\n");
  resetDrivingValues();
}

void update_vcu_state_driving() {
  // Send torque commands
  torque_command = calcTorque(pedalbox.accel_1, pedalbox.accel_2);

  // sendTorqueCmdMsg(torque_command);

  // TODO: Debug CAN BUSY? (2) errors that stop us from reading vals.
  static Time_T last_print = 0;
  if (HAL_GetTick() - last_print > print_period) {
    printf("A1:%d A2:%d T:%d\r\n", pedalbox.accel_1, pedalbox.accel_2, torque_command);

    last_print = HAL_GetTick();
  }
}
