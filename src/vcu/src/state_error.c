#include "state_error.h"

static ERROR_STATE_T currentState = NO_ERROR_STATE;

void init_error_state(void) {
  set_error_state(NO_ERROR_STATE);
}

ERROR_STATE_T set_error_state(ERROR_STATE_T newState) {
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

#define TRANSITION_FATAL() \
  if (update_fatal_faults()) set_error_state(FATAL_ERROR_STATE);
#define TRANSITION_RECOVERABLE() \
  if (!update_fatal_faults() && update_recoverable_faults()) set_error_state(RECOVERABLE_ERROR_STATE);
#define TRANSITION_NERR() \
  if (!update_fatal_faults() && !update_recoverable_faults()) set_error_state(NO_ERROR_STATE);

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

  if (get_vcu_state() == VCU_STATE_DRIVING)
    if (buttons.RTD)
      set_vcu_state(VCU_STATE_RTD);

  // Command both contactors open to avoid subsequent timeout faults
  if (any_recoverable_gate_fault()) {
    openLowSideContactor();
    openHighSideContactor();

    // Resume from LV state (precharge needs to happen again)
    // This will let the car try to go through precharge again
    // by flipping the DRIVER_RST pin.
    if ((get_vcu_state() != VCU_STATE_ROOT) &&
        (get_vcu_state() != VCU_STATE_LV)) set_vcu_state(VCU_STATE_LV);
  }

  // If no faults appear it means we've cleared.
  TRANSITION_NERR();

  handle_recoverable_fault();
}

void enter_fatal_error_state(void) {
  printf("[ERROR FSM : FATAL_ERROR_STATE] ENTERED!\r\n");
}

void update_fatal_error_state(void) {
  handle_fatal_fault();

  send_VCU();

  set_vcu_state(VCU_STATE_ROOT);

  TRANSITION_RECOVERABLE();
  TRANSITION_NERR();
}

ERROR_STATE_T get_error_state(void) {
  return currentState;
}

bool error_may_advance(void) {
  // If we're in a recoverable state and the only recoverable error is
  // the shutdown gate then we can advance the VCU state, knowing that it is
  // in VCU_STATE_LV due to update_recoverable_error_state.

  (void)update_recoverable_faults();

  bool recov_is_gate = get_error_state() == RECOVERABLE_ERROR_STATE &&
                       recoverable_faults.gate                      &&
                       !recoverable_faults.heartbeat                &&
                       !recoverable_faults.conflict                 &&
                       !recoverable_faults.contactor                &&
                       !any_recoverable_transient_gate_fault();

  return (get_error_state() == NO_ERROR_STATE) || recov_is_gate;
}
