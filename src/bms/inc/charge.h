#include "state_types.h"

#ifndef _CHARGE_H
# define _CHARGE_H

void Charge_Init(BMS_STATE_T *state);

void Charge_Step(BMS_INPUT_T  *input,
                 BMS_STATE_T  *state,
                 BMS_OUTPUT_T *output);

#endif // ifndef _CHARGE_H
