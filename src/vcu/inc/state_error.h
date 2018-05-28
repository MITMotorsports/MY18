#ifndef __STATE_ERROR_H
#define __STATE_ERROR_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include <stdbool.h>

#include "fault.h"
#include "state.h"
#include "extern.h"


// ERROR STATES
typedef enum {
  NO_ERROR_STATE = 0,
  RECOVERABLE_ERROR_STATE = 1,
  FATAL_ERROR_STATE = 2
} ERROR_STATE_T;



// INTERACTION FUNCTIONS
void          init_error_state(void);
void          advance_error_state(void);
ERROR_STATE_T set_error_state(ERROR_STATE_T newState);

ERROR_STATE_T get_error_state(void);

bool error_may_advance(void);

// PRIVATE FUNCTIONS


#endif // ifndef __STATE_ERROR_H
