#ifndef __LV_ONLY_H
#define __LV_ONLY_H

#include "stdio.h"
#include <stdbool.h>

#include "state.h"
#include "gpio.h"

#define DRIVER_RESET_LATCHING_TIME 2 // millis
#define CLOSE_VCU_GATE_TIME        2 // millis

void initLVOnly();
void loopLVOnly();

#endif // ifndef __LV_ONLY_H
