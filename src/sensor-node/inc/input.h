#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"
#include "chip.h"

#include "adc.h"

#define ADC_UPDATE_PERIOD_MS 1

void Input_fill_input(void);
void Input_handle_interrupt(uint32_t msTicks, uint32_t curr_tick, Wheel_T wheel);

#endif // _INPUT_H_
