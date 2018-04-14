#ifndef __CONTACTORS_H
#define __CONTACTORS_H

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "gpio.h"

void openLowSideContactor(void);
void closeLowSideContactor(void);

void openHighSideContactor(void);
void closeHighSideContactor(void);

#endif // ifndef __CONTACTORS_H
