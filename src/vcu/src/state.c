#include "state.h"

void init_states(void) {
  init_error_state();
  init_vcu_state();
}

void advance_states(void) {
  advance_error_state();

  // If we have errored, pause VCU FSM execution.
  if (get_error_state() == NO_ERROR_STATE) {
    advance_vcu_state();
  }
}
