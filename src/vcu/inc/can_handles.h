#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "state.h"

void handleBreakThrottleMsg(Frame* msg);

#endif