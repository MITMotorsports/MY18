#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

void State_initialize(State_T *state);
void State_update_state(Input_T *input, State_T *state);

#endif // STATE_H
