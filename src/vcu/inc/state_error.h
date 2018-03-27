#ifndef __STATE_ERROR_H
#define __STATE_ERROR_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include <stdbool.h>

#include "fault.h"
#include "extern.h"


// ERROR STATES
typedef enum {
  NO_ERROR_STATE,
  RECOVERABLE_ERROR_STATE,
  FATAL_ERROR_STATE
} ERROR_STATE_T;


// CONTAINER DEFINITIONS
typedef struct {
  bool HEARTBEAT_ERROR;
  bool SHUTDOWN_ERROR; // Only includes SHUTDOWN_SENSE(_GATE) AKA ESD
} RECOVERABLE_ERROR_T;

typedef struct {
  bool GATE_ERROR;
  bool CONFLICT_ERROR;
} FATAL_ERROR_T;


// CONTAINER DECLARATIONS
extern RECOVERABLE_ERROR_T recoverable_errors;
extern FATAL_ERROR_T fatal_errors;


// INTERACTION FUNCTIONS
void          init_error_state(void);
void          transition_error_state(void);
ERROR_STATE_T set_error_state(ERROR_STATE_T newState);

ERROR_STATE_T current_error_state(void);

// PRIVATE FUNCTIONS


#endif // ifndef __STATE_ERROR_H
