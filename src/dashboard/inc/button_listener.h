#ifndef _BUTTON_LISTENER_H_
#define _BUTTON_LISTENER_H_

typedef enum {
    BUTTON_ACTION_TAP;
    BUTTON_ACTION_HOLD;
    BUTTON_ACTION_NONE;
} btn_action_t;

typedef struct {
    bool is_pressed;
    bool lockout;
    btn_action_t action;
    int press_start_ms;
} button_state_t;

void update_button_state(button_state_t *state, uint8_t port, uint8_t pin);

#endif
