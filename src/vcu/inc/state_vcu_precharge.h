#ifndef __PRECHARGE_H
#define __PRECHARGE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"

#define PRECHARGE_DEAD_RECKONING_TIME   5 * 1000
#define PRECHARGE_TOO_LONG_DURATION 10 * 1000
#define DC_BUS_VOLTAGE_SCALE_FACTOR  10
#define SHOW_VOLTAGE_FREQUENCY_UPDATE   500

void    initPrecharge();

void    loopPrecharge();

int16_t calcTargetVoltage(int16_t packVoltage);

uint32_t prechargeStartTime;
int16_t  targetVoltage;
uint32_t showVoltageTime;


#endif // ifndef __PRECHARGE_H
