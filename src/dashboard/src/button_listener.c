#include "button_listener.h"

#include "board.h"

#define TAP_HOLD_CUTOFF 400

void init_button_state(button_state_t *state) {
    state->is_pressed = false;
    state->action     = BUTTON_ACTION_NONE;

    state->edge         = false;
    state->rising_edge  = false;
    state->falling_edge = false;
    state->hold_edge    = false;

    state->setup_time   = 10;
    state->hold_time    = 10;
}

void update_button_state(button_state_t *state, bool newval) {
    // Initialization
    btn_action_t prev_action = state->action;

    state->action = BUTTON_ACTION_NONE;
    state->rising_edge = false;
    state->falling_edge = false;
    int duration = msTicks - state->last_edge_ms;

    // Debounce newval
    if (duration < (newval ? state->setup_time : state->hold_time))
        newval = state->is_pressed;

    state->edge = state->is_pressed != newval;
    if (state->edge) {
        state->rising_edge = newval;
        state->falling_edge = !newval;
        state->last_edge_ms = msTicks;
    }

    state->is_pressed = newval;

    // Set actions
    state->hold_edge = false;
    if (state->is_pressed && duration > TAP_HOLD_CUTOFF) {
        state->action = BUTTON_ACTION_HOLD;
        if (prev_action != BUTTON_ACTION_HOLD) {
            state->hold_edge = true;
        }
    }

    if (duration < TAP_HOLD_CUTOFF && state->falling_edge) {
        state->action = BUTTON_ACTION_TAP;
    }
}
