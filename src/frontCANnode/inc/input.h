#ifndef _INPUT_H_
#define _INPUT_H_

#include "types.h"

void Input_initialize(Input_T *input);
void Input_fill_input(Input_T *input);
void Input_handle_interrupt(Speed_Input_T *speed, uint32_t msTicks, uint32_t curr_tick, Wheel_T wheel);

#endif // _INPUT_H_
