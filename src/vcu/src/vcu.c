
#include "vcu.h"

void setupVCU() {
    initVCUState();
}

void loopVCU() {
    if (HAL_GetTick() - board_heartbeats_state.frontCanNode > CAN_DEAD_DURATION) {
        HAL_Delay(1); // Just for debugging
    }

    // Send torque commands
    VCU_BrakeAndThrottle localBTState = brake_and_throttle_state;

    update_implausibility(localBTState, &implaus_conflict_state);
    update_brake_throttle_conflict(localBTState, &implaus_conflict_state, board_heartbeats_state.frontCanNode);

    // do some calc
    sendTorqueCmdMsg(10);
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
