#include "charge.h"

void Charge_Init(BMS_STATE_T *state) {
  state->charge_state = BMS_CHARGE_OFF;
}

void Charge_Step(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output) {
  // fan stuff here:


  switch (input->mode_request) {
  case BMS_SSM_MODE_CHARGE:
    break;

    // start out in charge off
    // go to charge init
    // go to calc balancing
    // if balancing bool is false
    // go to charge done
  }
}
