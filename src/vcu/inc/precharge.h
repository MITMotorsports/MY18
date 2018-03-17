
#ifndef __PRECHARGE_H
#define __PRECHARGE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"

#define PRECHARGE_TOO_LONG_DURATION		10 * 1000 // seconds
#define DC_BUS_VOLTAGE_SCALE_FACTOR		10

void initPrecharge();

void loopPrecharge();

int16_t calcTargetVoltage(int16_t packVoltage);

uint32_t prechargeStartTime;
int16_t targetVoltage;


#endif
