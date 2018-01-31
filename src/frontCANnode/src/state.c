#include "state.h"
#include "types.h"
#include "rules.h"

#include <Can_Library.h>

// Both of these are in millisecods
const uint8_t can_output_period = 1000 / FRONTCANNODEOUTPUT__freq;
const uint8_t can_wheel_speed_period = 1000 / FRONTCANNODEWHEELSPEED__freq;

void update_torque_state(ADC_Input_T *adc, State_T *state);
void update_can_state(Can_Output_State_T *can_output, uint32_t msTicks);

bool period_reached(uint32_t start, uint32_t period, uint32_t msTicks);
void update_can_output(Can_Output_State_T *can_output, uint32_t msTicks);
void update_can_wheel_speed(Can_Output_State_T, uint32_t msTicks);

void State_initialize(State_T *state) {
  state->rules->implausibility_observed = false;
  state->rules->implausibility_time_ms = 0;
  state->rules->implausibility_reported = false;
  state->rules->has_brake_throttle_conflict = false;

  state->can_output->send_output_msg = false;
  state->can_output->send_wheel_speed_msg = false;
}

void State_update_state(Input_T *input, State_T *state) {
  Rules_update_implausibility(input->adc, state->rules, input->msTicks);
  Rules_update_conflict(input, state->rules);
  update_torque_state(input->adc, state);
  update_can_state(state->can_output, input->msTicks);
}

void update_torque_state(ADC_Input_T *adc, State_T *state) {
  uint16_t min_accel = (adc->accel_1 > adc->accel_2) ? adc->accel_2 : adc->accel_1;

  if (state->rules->implausibility_reported || state->rules->has_brake_throttle_conflict) {
    state->torque->requested_torque = 0;
  }
}

void update_can_state(Can_Output_State_T *can_output, uint32_t msTicks) {
  if(period_reached(can_output->can_output_ms, can_output_period, msTicks)) {
    can_output->can_output_ms = msTicks;
    can_output->send_output_msg = true;
  }

  if(period_reached(can_output->can_wheel_speed_ms, can_wheel_speed_period, msTicks)) {
    can_output->can_wheel_speed_ms = msTicks;
    can_output->send_wheel_speed_msg = true;
  }
}

bool period_reached(uint32_t start, uint32_t period, uint32_t msTicks) {
  const uint32_t next_time = start + period;
  return next_time < msTicks;
}
