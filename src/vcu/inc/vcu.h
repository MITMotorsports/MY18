#ifndef __VCU_H
#define __VCU_H

#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "state.h"

#define CAN_DEAD_DURATION 100

void setupVCU();

void loopVCU(USART_HandleTypeDef* USARTHandle);

void handleCanVCU(CAN_HandleTypeDef* CanHandle);

#endif