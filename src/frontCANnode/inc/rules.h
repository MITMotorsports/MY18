#ifndef _RULES_H_
#define _RULES_H_

#include "types.h"

#include <stdint.h>

void Rules_update_implausibility(ADC_Input_T *adc, Rules_State_T *rules, uint32_t msTicks);
void Rules_update_conflict(Input_T *input, Rules_State_T *rules);

#endif //_RULES_H_
