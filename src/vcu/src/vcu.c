
#include "vcu.h"

void setupVCU() {
    initVCUState();
    setupErrorState();

    lastDashMsgTime = HAL_GetTick();
}

void beforeLoop() {
    updateErrorState(); // Check for errors
}

void afterLoop() {
    if (HAL_GetTick() - lastDashMsgTime >= SEND_DASH_MSG_WAIT_DURATION) {
        sendDashMsg();
        lastDashMsgTime = HAL_GetTick();
    }
}

void loopVCU() {
    beforeLoop();   // CAUTION: REMOVING THIS REMOVES ERROR CHECKING

    switch (carMode) {
        case CAR_STATE_LV_ONLY:
            loopLVOnly();
            break;
        case CAR_STATE_PRECHARGING:
            loopPrecharge();
            break;
        case CAR_STATE_CHARGE_FAULT:
            loopChargeFault();
            break;
        case CAR_STATE_READY_TO_DRIVE:
            loopReadyToDrive();
            break;
        case CAR_STATE_DRIVING:
            loopDriving();
            break;
        case CAR_STATE_CONTACTOR_FAULT:
            loopContactorFault();
            break;

        default:
            break;
    }

    afterLoop();
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
