#ifndef __LV_ONLY_H
#define __LV_ONLY_H

#include "stdio.h"
#include <stdbool.h>

#include "state.h"
#include "gpio.h"

#define DRIVER_RESET_LATCHING_TIME 2 // millis
#define CLOSE_VCU_GATE_TIME        2 // millis

void enter_vcu_state_lv(void);
void update_vcu_state_lv(void);

#endif // ifndef __LV_ONLY_H
