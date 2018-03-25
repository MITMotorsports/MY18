
#ifndef __LV_ONLY_H
#define __LV_ONLY_H

#include "stdio.h"
#include "state.h"
#include "error_state.h"
#include "contactors.h"
#include <stdbool.h>

#define DRIVER_RESET_LATCHING_TIME 2 // millis
#define CLOSE_VCU_GATE_TIME        2 // millis

bool latchingDriverReset;
bool closingVCUFET;
uint32_t timeSinceLatchSettingStart;

void initLVOnly();
void loopLVOnly();

#endif // ifndef __LV_ONLY_H
