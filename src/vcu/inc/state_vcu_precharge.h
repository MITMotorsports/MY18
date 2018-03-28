#ifndef __PRECHARGE_H
#define __PRECHARGE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "extern.h"
#include "state.h"
#include "contactors.h"

void enter_vcu_state_precharge(void);
void update_vcu_state_precharge(void);

Voltage_T calculate_precharge_target(void);

#endif // ifndef __PRECHARGE_H
