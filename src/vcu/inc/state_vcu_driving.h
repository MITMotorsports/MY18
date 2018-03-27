#ifndef __DRIVING_H
#define __DRIVING_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"
#include "can_handles.h"
#include "fault_conflicts.h"
#include "torque_calc.h"

void resetDrivingValues();
void initDriving();
void loopDriving();

uint16_t torque_command;

#endif // ifndef __DRIVING_H
