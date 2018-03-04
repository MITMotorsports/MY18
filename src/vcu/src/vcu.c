
#include "vcu.h"

void setupVCU() {
    initVCUState();
}

void loopVCU(USART_HandleTypeDef* uhandle) {
    if (HAL_GetTick() - board_heartbeats_state.frontCanNode > CAN_DEAD_DURATION) {
        HAL_Delay(1); // Just for debugging
    }

    // Send torque commands
    VCU_BrakeAndThrottle localBTState = brake_and_throttle_state;

    update_implausibility(localBTState, &implaus_conflict_state);
    update_brake_throttle_conflict(localBTState, &implaus_conflict_state, board_heartbeats_state.frontCanNode);

    printf("\r\n");
    printf("\r\nhas_brake_throttle_conflict: %d\r\n", implaus_conflict_state.has_brake_throttle_conflict);
    printf("\r\nobserved_implausibility: %d\r\n", implaus_conflict_state.observed_implausibility);
    printf("\r\nactual_implausibility: %d\r\n", implaus_conflict_state.actual_implausibility);
    printf("\r\nimplausibility_ticks: %d\r\n", implaus_conflict_state.implausibility_ticks);
    printf("\r\n");
    HAL_Delay(1000);

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
