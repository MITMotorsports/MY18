#ifndef __DRIVING_H
#define __DRIVING_H

#include "stdio.h"

#include "state_vcu.h"
#include "can_handles.h"
#include "torque_calc.h"

void resetDrivingValues();
void enter_vcu_state_driving();
void update_vcu_state_driving();

#endif // ifndef __DRIVING_H
