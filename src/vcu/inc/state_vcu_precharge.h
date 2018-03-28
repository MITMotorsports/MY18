#ifndef __PRECHARGE_H
#define __PRECHARGE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "extern.h"
#include "state.h"
#include "contactors.h"


void    initPrecharge(void);
void    loopPrecharge(void);


int16_t calcTargetVoltage(int16_t pack);


#endif // ifndef __PRECHARGE_H
