#ifndef __STATE_ERROR_H
#define __STATE_ERROR_H

#include <stdbool.h>
#include "gpio.h"
#include "state.h"

// INTERACTION FUNCTIONS
void init_error_state(void);
void advance_error_state(void);
void set_error_state(bool newState);

bool get_error_state(void);

bool error_may_advance(void);

#endif // ifndef __STATE_ERROR_H
