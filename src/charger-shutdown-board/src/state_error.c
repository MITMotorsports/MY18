#include "state_error.h"

static bool currentState = false;

void init_error_state(void) {
  set_error_state(false);
}

void set_error_state(bool newState) {
  if (newState == ERROR_STATE_ERROR) {
    enter_error_state();
  } else {
    enter_no_error_state();
  }
  currentState = newState;
}

void advance_error_state(void) {
  if (any_gate_fault()) {
    if (currentState != ERROR_STATE_ERROR) set_error_state(ERROR_STATE_ERROR);

    if (get_csb_state() != CSB_STATE_ROOT) set_csb_state(CSB_STATE_ROOT);
  }
  else {
    if (currentState == ERROR_STATE_ERROR) set_error_state(ERROR_STATE_NONE);
  }

  // TODO: Add an error on a persistent low side not closing
  // any_gate_fault() && !bms_state.L_contactor_closed

  if (currentState == ERROR_STATE_ERROR) handle_error();
}

void enter_error_state(void) {
  Board_Println("[ERROR FSM : ERROR STATE] ENTERED!");
}

void enter_no_error_state(void) {
  Board_Println("[ERROR FSM : NO ERROR STATE] ENTERED!");
}

inline bool get_error_state(void) {
  return currentState;
}

void handle_error(void) {
  Board_Pin_Set(PIN_PRECHARGE, false);
}
