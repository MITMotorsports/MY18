
#ifndef __PRECHARGE_H
#define __PRECHARGE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"

#define PRECHARGE_TOO_LONG_DURATION		10 * 1000 // seconds
#define DC_BUS_VOLTAGE_THRESHOLD		// NOPE, NEED TO GET BMS VOLTAGE TO CALC 90% OF THAT
#define DC_BUS_VOLTAGE_SCALE_FACTOR		10

void initPrecharge();

void loopPrecharge();

uint32_t prechargeStartTime;


#endif
