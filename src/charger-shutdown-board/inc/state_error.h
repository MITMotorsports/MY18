#include <stdbool.h>

#include "state.h"
#include "gpio.h"

static bool currentState = false;

void init_error_state(void) {
  set_error_state(false);
}

void set_error_state(bool newState) {
  if (newState) {
    // ERROR PRESENT
  }
  else {
    // NO ERRORS
  }

  currentState = newState;
}

void advance_error_state(void) {
  if (currentState) {
    // ERROR PRESENT
    Board_Println("[ERROR FSM : ERROR] ENTERED!");
  }
  else {
    // NO ERRORS
    Board_Println("[ERROR FSM : NONE] ENTERED!");
  }
}

inline bool get_error_state(void){
	return currentState;
}

bool error_may_advance(void) {

}
