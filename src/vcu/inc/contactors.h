#ifndef __CONTACTORS_H
#define __CONTACTORS_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"

void initLowSideContactor();
void initHighSideContactor();

void openLowSideContactor();
void closeLowSideContactor();

void openHighSideContactor();
void closeHighSideContactor();

#endif // ifndef __CONTACTORS_H
