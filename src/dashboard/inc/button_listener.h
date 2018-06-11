#ifndef _BUTTON_LISTENER_H_
#define _BUTTON_LISTENER_H_

#include <stdbool.h>

// tap: brief button press
// hold: extended button press
// none: nothing happened
typedef enum {
    BUTTON_ACTION_TAP,
    BUTTON_ACTION_HOLD,
    BUTTON_ACTION_NONE
} btn_action_t;

// action field is only not BUTTON_ACTION_NONE
// for a single update cycle... so check the value
// every time you update.
typedef struct {
    bool is_pressed;
    bool lockout;
    btn_action_t action;
    int press_start_ms;

    bool edge;
    bool rising_edge;
    bool falling_edge;
} button_state_t;

void init_button_state(button_state_t *state);

// update the button state. `newval` == true means button pushed.
void update_button_state(button_state_t *state, bool newval);

#endif
