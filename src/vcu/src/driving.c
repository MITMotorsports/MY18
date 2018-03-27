#include "driving.h"

void resetDrivingValues() {
  torque_command                   = 0;
  pedalbox.accel_1 = 0;
  pedalbox.accel_2 = 0;
}

void initDriving() {
  resetDrivingValues();
}

void loopDriving() {
  // Send torque commands
  Pedalbox_T localBTState = pedalbox;

  torque_command = calcTorque(localBTState.accel_1, localBTState.accel_2);

  if (conflicts.actual_implausibility ||
      conflicts.has_brake_throttle_conflict) {
    torque_command = 0;
  }

  // printf("\r\nbrkcflct: %d\r\n",
  // conflicts.has_brake_throttle_conflict);
  // printf("\r\nhrt: %d\r\n", heartbeats.frontCanNode);
  // printf("\r\naccel: %d\r\n", localBTState.accel_1);
  // printf("\r\ntrq: %d\r\n", torque_command);
  // printf("\r\nticks: %d\r\n", HAL_GetTick());

  // HAL_Delay(50);

  sendTorqueCmdMsg(torque_command);
}
