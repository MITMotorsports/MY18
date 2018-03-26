#ifndef __VCU_H
#define __VCU_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "state.h"
#include "lv_only.h"
#include "precharge.h"
#include "charge_fault.h"
#include "ready_to_drive.h"
#include "driving.h"
#include "contactor_fault.h"
#include "error_state.h"
#include "extern.h"

void setupVCU();

void beforeLoop();

void afterLoop();

void loopVCU();

void handleCanVCU(CAN_HandleTypeDef *CanHandle);

#endif // ifndef __VCU_H
