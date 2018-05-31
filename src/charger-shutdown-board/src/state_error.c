#include "state_error.h"

void enter_error_state(void);
void enter_no_error_state(void);

static bool currentState = false;

void init_error_state(void) {
  set_error_state(false);
}


void set_error_state(bool newState) {
  if (newState) {
    enter_error_state();
  } else {
    enter_no_error_state();
  }
  currentState = newState;
}


void advance_error_state() {
  if (any_gate_fault() || (get_csb_state() != CSB_STATE_ROOT && !bms_state.L_contactor_closed)) {
      if (!currentState) {
        set_error_state(true);
        Board_Pin_Set(PIN_PRECHARGE, 0);
        send_ChargerCommand(0);
        if (get_csb_state() != CSB_STATE_ROOT) {
          set_csb_state(CSB_STATE_ROOT);
        }
      }
  } else {
    if (currentState) {
      set_error_state(false);
    }
  }
}


void enter_error_state(void) {
  Board_Println("[ERROR FSM : ERROR STATE] ENTERED!");
}


void enter_no_error_state(void) {
  Board_Println("[ERROR FSM : NO ERROR STATE] ENTERED!");
}


bool get_error_state(void) {
  return currentState;
}
