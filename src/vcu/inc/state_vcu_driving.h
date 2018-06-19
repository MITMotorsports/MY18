#ifndef __DRIVING_H
#define __DRIVING_H

#include "stdio.h"

#include "state_vcu.h"
#include "can_handles.h"
#include "controls.h"

#define AA_SETUP 10
#define AA_HOLD 10

void enter_vcu_state_driving(void);
void update_vcu_state_driving(void);

#endif // ifndef __DRIVING_H
