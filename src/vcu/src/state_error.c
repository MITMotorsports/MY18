#include "state_error.h"

static ERROR_STATE_T currentState = NO_ERROR_STATE;
bool not_yet_master_rst           = true;

// Implicitly initialize error structs to false (no errors).
RECOVERABLE_ERROR_T recoverable_errors = {};
FATAL_ERROR_T fatal_errors             = {};


void init_error_state(void) {
  set_error_state(NO_ERROR_STATE);
}

ERROR_STATE_T set_error_state(ERROR_STATE_T newState) {
  if (currentState == FATAL_ERROR_STATE) {
    printf("[ERROR FSM] Attempted to exit from FATAL_ERROR_STATE! Ignoring...");
    return currentState;
  }

  switch (newState) {
  case NO_ERROR_STATE:
    enter_no_error_state();
    break;

  case RECOVERABLE_ERROR_STATE:
    enter_recoverable_error_state();
    break;

  case FATAL_ERROR_STATE:
    enter_fatal_error_state();
    break;
  }

  currentState = newState;
  return currentState;
}

// Actual transitions happen via set_error_state inside the updates.
void advance_error_state(void) {
  switch (currentState) {
  case NO_ERROR_STATE:
    update_no_error_state();
    break;

  case RECOVERABLE_ERROR_STATE:
    update_recoverable_error_state();
    break;

  case FATAL_ERROR_STATE:
    update_fatal_error_state();
    break;
  }
}

void enter_no_error_state(void) {
  printf("[ERROR FSM] Entered NO_ERROR_STATE!\r\n");
}

void update_no_error_state(void) {
  if (any_fatal_gate_faults() ||
      any_fatal_precharge_fault() ||
      any_fatal_contactor_faults() ||
      any_fatal_conflict_faults())
  {
    set_error_state(FATAL_ERROR_STATE);
  }

  if (any_recoverable_gate_fault() ||
      any_recoverable_heartbeat_faults() ||
      any_recoverable_conflict_faults())
  {
    set_error_state(RECOVERABLE_ERROR_STATE);
  }
}

void enter_recoverable_error_state(void) {
  printf("[ERROR FSM : RECOVERABLE_ERROR_STATE] ENTERED!\r\n");
}

void update_recoverable_error_state(void) {
  // We do not have recoverers yet.
  handle_fatal_fault();
}

void enter_fatal_error_state(void) {
  printf("[ERROR FSM : FATAL_ERROR_STATE] ENTERED!\r\n");
}

void update_fatal_error_state(void) {
  // We ded man.
  handle_fatal_fault();
}

ERROR_STATE_T get_error_state(void) {
  return currentState;
}
