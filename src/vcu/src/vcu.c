
#include "vcu.h"

uint16_t torque_command;

void setupVCU() {
    initVCUState();
}

void loopVCU(USART_HandleTypeDef* uhandle) {
    if (HAL_GetTick() - board_heartbeats_state.frontCanNode > CAN_DEAD_DURATION) {
        torque_command = 0;
        brake_and_throttle_state.accel_1 = 0;
        brake_and_throttle_state.accel_2 = 0;
    }

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

    // HAL_Delay(500);

    sendTorqueCmdMsg(torque_command, implaus_conflict_state.has_brake_throttle_conflict);
}

void handleCanVCU(CAN_HandleTypeDef* CanHandle) {
    Frame frame;

    lastRxMsgToFrame(&frame);

    can0_T msgForm;
    msgForm = identify_can0(&frame);

    switch (msgForm) {
        case can0_FrontCanNodeBrakeThrottle:
          handleBrakeThrottleMsg(&frame);
          break;

        default:
          break;
    }
}
