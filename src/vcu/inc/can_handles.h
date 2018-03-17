#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "state.h"

#define CAN_DEAD_DURATION 400 // TODO: Should be able to read this from CAN spec for each message

void handleBrakeThrottleMsg(Frame* msg);

void handleMCVoltageMsg(Frame* msg);

void handleBMSHeartbeatMsg(Frame* msg);

void sendTorqueCmdMsg(int16_t torque, int16_t break_and_throttle_conflict);

#endif
