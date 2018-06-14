#include "button_listener.h"

#include "board.h"

#define TAP_HOLD_CUTOFF 300

void init_button_state(button_state_t *state) {
    state->is_pressed = false;
    state->action     = BUTTON_ACTION_NONE;

    state->edge         = false;
    state->rising_edge  = false;
    state->falling_edge = false;
}

void update_button_state(button_state_t *state, bool newval) {
    // Initialization
    state->action = BUTTON_ACTION_NONE;
    state->rising_edge = false;
    state->falling_edge = false;
    int duration = msTicks - state->last_edge_ms;

    // Set states
    // is_pressed also indicates our previous state, so if it does not equal the
    // new state, it must be an edge
    state->edge = state->is_pressed != newval;
    if (state->edge) {
      state->rising_edge = newval;
      state->falling_edge = !newval;
      state->last_edge_ms = msTicks;
    }

    state->is_pressed = newval;

    // Set actions
    if (state->is_pressed && duration > TAP_HOLD_CUTOFF) {
      state->action = BUTTON_ACTION_HOLD;
    }

    if (duration < TAP_HOLD_CUTOFF && state->falling_edge) {
        state->action = BUTTON_ACTION_TAP;
    }
}
