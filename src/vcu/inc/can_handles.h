#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "state.h"

void handleBrakeThrottleMsg(Frame* msg);

void sendTorqueCmdMsg(int16_t torque, int16_t break_and_throttle_conflict);

#endif
