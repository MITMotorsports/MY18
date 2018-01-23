#include "input.h"
#include "adc.h"

#include <Can_Library.h>

#define ADC_UPDATE_PERIOD_MS 10

// TODO these are garbage numbers (taken from MY17, need fixing)
#define ACCEL_1_LOWER_BOUND 105
#define ACCEL_1_UPPER_BOUND 645
#define ACCEL_2_LOWER_BOUND 71
#define ACCEL_2_UPPER_BOUND 340

void update_adc(Input_T *input);
void update_can(Input_T *input);

void can_process_vcu_dash(Input_T *input);

uint16_t transform1(uint32_t accel_1_raw);
uint16_t transform2(uint32_t accel_2_raw);

// TODO Add wheel speed stuff

void Input_initialize(Input_T *input) {
  input->adc->accel_1 = 0;
  input->adc->accel_2 = 0;
  input->adc->accel_1_raw = 0;
  input->adc->accel_2_raw = 0;
  input->adc->brake_1 = 0;
  input->adc->brake_2 = 0;

  input->can_input->limp_state = CAN_VCUTODASH_LIMP_NORMAL; // Based on CAN spec
}

void Input_fill_input(Input_T *input) {
  update_adc(input);
  update_can(input);
}

void update_adc(Input_T *input) {
  ADC_Input_T *adc = input->adc;
  uint32_t next_updated = adc->last_updated + ADC_UPDATE_PERIOD_MS;

  if (next_updated < input->msTicks) {
    adc->accel_1_raw = ADC_Read(ACCEL_1_CHANNEL);
    adc->accel_2_raw = ADC_Read(ACCEL_2_CHANNEL);
    adc->brake_1 = ADC_Read(BRAKE_1_CHANNEL);
    adc->brake_2 = ADC_Read(BRAKE_2_CHANNEL);
    adc->last_updated = input->msTicks;

    adc->accel_1 = transform1(adc->accel_1_raw);
    adc->accel_2 = transform2(adc->accel_2_raw);
  }
}

void update_can(Input_T *input) {
  Can_MsgID_T msgID = Can_MsgType();
  switch(msgID) {
    case Can_VcuToDash_Msg:
      can_process_vcu_dash(input);
      break;
    default:
      break;
  }
}

void can_process_vcu_dash(Input_T *input) {
  Can_VcuToDash_T msg;
  Can_VcuToDash_Read(&msg);
  input->can_input->limp_state = msg.limp_state;
}

uint16_t linear_transfer_fn(uint32_t reading, uint16_t desired_width, uint16_t lower_bound, uint16_t upper_bound) {
  // Mostly copied from MY17

  // Ensure reading is within expected range
  if (lower_bound > reading) reading = lower_bound;
  if (upper_bound < reading) reading = upper_bound;

  // Make reading between 0 and diff
  const uint16_t diff = upper_bound - lower_bound;
  reading = reading - lower_bound;

  // Now scale from [0:diff] to [0:desired_width].
  // Note: it's critical that reading be a 32 bit int because otherwise this line will cause overflow!
  reading = reading * desired_width;
  reading = reading / diff;

  // This cast is safe because we have asserted reading is in range [0:desired_width] in previous lines
  uint16_t short_val = (uint16_t)(reading);
  return short_val;
}

uint16_t transform1(uint32_t accel_1_raw) {
  // Scale to between 0 and 1000
  return linear_transfer_fn(accel_1_raw, 1000, ACCEL_1_LOWER_BOUND, ACCEL_1_UPPER_BOUND);
}

uint16_t transform2(uint32_t accel_2_raw) {
  // Scale to between 0 and 1000
  return linear_transfer_fn(accel_2_raw, 1000, ACCEL_2_LOWER_BOUND, ACCEL_2_UPPER_BOUND);
}
