
#include "driving.h"

void resetDrivingValues() {
    torque_command = 0;
    brake_and_throttle_state.accel_1 = 0;
    brake_and_throttle_state.accel_2 = 0;
}

void initDriving() {
	resetDrivingValues();
}

void loopDriving() {
    // Send torque commands
    VCU_BrakeAndThrottle localBTState = brake_and_throttle_state;

    update_implausibility(localBTState, &implaus_conflict_state, board_heartbeats_state.frontCanNode);
    update_brake_throttle_conflict(localBTState, &implaus_conflict_state);

    torque_command = calcTorque(localBTState.accel_1, localBTState.accel_2);

    if (implaus_conflict_state.actual_implausibility || implaus_conflict_state.has_brake_throttle_conflict) {
        torque_command = 0;
    }

    // printf("\r\nbrkcflct: %d\r\n", implaus_conflict_state.has_brake_throttle_conflict);
    // printf("\r\nhrt: %d\r\n", board_heartbeats_state.frontCanNode);
    // printf("\r\naccel: %d\r\n", localBTState.accel_1);
    // printf("\r\ntrq: %d\r\n", torque_command);
    // printf("\r\nticks: %d\r\n", HAL_GetTick());

    // HAL_Delay(50);

    sendTorqueCmdMsg(torque_command);
}
