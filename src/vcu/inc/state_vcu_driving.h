#ifndef __DRIVING_H
#define __DRIVING_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"
#include "can_handles.h"
#include "fault_conflicts.h"
#include "torque_calc.h"

void resetDrivingValues();
void enter_vcu_state_driving();
void update_vcu_state_driving();

uint16_t torque_command;

#endif // ifndef __DRIVING_H
