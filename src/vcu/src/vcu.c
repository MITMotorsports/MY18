
#include "vcu.h"

void setupVCU() {
    initVCUState();
}

void loopVCU() {
    // if (shouldSend) {

 //      can0_MC_Command_T msg;
 //      msg.torque_limit = 0;
 //      msg.discharge_enabled = 0;
 //      msg.direction_is_counterclockwise = 0;
 //      msg.speed = 0;
 //      msg.inverter_enabled = 1;
 //      msg.torque = 10000;

 //      can0_MC_Command_Write(&msg);

 //      shouldSend = false;
 //    }

    if (HAL_GetTick() - board_heartbeats_state.frontCanNode > CAN_DEAD_DURATION) {
        HAL_Delay(1); // Just for debugging
    }
}

void handleCanVCU(CAN_HandleTypeDef* CanHandle) {
    Frame frame;

    lastRxMsgToFrame(&frame);

    can0_T msgForm;
    msgForm = identify_can0(&frame);

    switch (msgForm) {
        case can0_FrontCanNodeBrakeThrottle:
          handleBreakThrottleMsg(&frame);
          break;

        default:
          break;
    }
}
