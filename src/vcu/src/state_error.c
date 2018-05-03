#include "state_error.h"

static ERROR_STATE_T currentState = NO_ERROR_STATE;

void init_error_state(void) {
  set_error_state(NO_ERROR_STATE);
}

ERROR_STATE_T set_error_state(ERROR_STATE_T newState) {
  if (currentState == FATAL_ERROR_STATE) {
    printf("[ERROR FSM] Attempted to exit from FATAL_ERROR_STATE! Ignoring...\r\n");
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

#define TRANSITION_FATAL() if (update_fatal_faults()) set_error_state(FATAL_ERROR_STATE);
#define TRANSITION_RECOVERABLE() if (update_recoverable_faults()) set_error_state(RECOVERABLE_ERROR_STATE);

void enter_no_error_state(void) {
  printf("[ERROR FSM : NO_ERROR_STATE] ENTERED!\r\n");
}

void update_no_error_state(void) {
  TRANSITION_FATAL();
  TRANSITION_RECOVERABLE();
}

void enter_recoverable_error_state(void) {
  printf("[ERROR FSM : RECOVERABLE_ERROR_STATE] ENTERED!\r\n");
}

void update_recoverable_error_state(void) {
  TRANSITION_FATAL();
  // If no recoverable faults appear it means we've cleared.
  if (!update_recoverable_faults()) set_error_state(NO_ERROR_STATE);

  handle_recoverable_fault();
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
