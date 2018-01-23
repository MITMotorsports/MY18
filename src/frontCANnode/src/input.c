#include "input.h"
#include "adc.h"

#include <Can_Library.h>

#define ADC_UPDATE_PERIOD_MS 10

// TODO these are garbage numbers (taken from MY17, need fixing)
#define ACCEL_1_LOWER_BOUND 105
#define ACCEL_1_UPPER_BOUND 645
#define ACCEL_2_LOWER_BOUND 71
#define ACCEL_2_UPPER_BOUND 340

// Some wheel speed stuff (copied from MY17)
#define WHEEL_SPEED_TIMEOUT_MS 100
#define WHEEL_SPEED_READ_PERIOD_MS 10

// Microsecond = 1 millionth of a second
#define MICROSECONDS_PER_SECOND_F 1000000.0
// 1000 millirevs = 1 rev
#define MILLIREVS_PER_REV_F 1000.0
// Pointless comment to not break pattern
#define SECONDS_PER_MINUTE 60

void update_adc(Input_T *input);
void update_wheel_speed(Speed_Input_T *speed, uint32_t msTicks);
void update_can(Input_T *input);

void can_process_vcu_dash(Input_T *input);

uint16_t transform1(uint32_t accel_1_raw);
uint16_t transform2(uint32_t accel_2_raw);


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
  update_wheel_speed(input->speed, input->msTicks);
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

uint32_t click_time_to_mRPM(uint32_t us_per_click) {
  const float us_per_rev = us_per_click * 1.0 * NUM_TEETH;

  const float s_per_rev = us_per_rev / MICROSECONDS_PER_SECOND_F;

  const float mrev_per_s = MILLIREVS_PER_REV_F / s_per_rev;

  const float mrev_per_min = mrev_per_s * SECONDS_PER_MINUTE;
  return (uint32_t)mrev_per_min;
}

void update_wheel_speed(Speed_Input_T *speed, uint32_t msTicks) {
  if (speed->last_speed_read_ms + WHEEL_SPEED_READ_PERIOD_MS < msTicks) {
    // Capture values
    speed->last_speed_read_ms = msTicks;
    uint8_t wheel;
    for(wheel = 0; wheel < NUM_WHEELS; wheel++) {
      const uint32_t count = speed->num_ticks[wheel];
      uint8_t idx;
      if (count > 0) {
        // If there are x ticks so far, the last tick is index (x - 1)
        idx = (count - 1) % NUM_TEETH;
      } else {
        idx = 0;
      }

      uint32_t moving_avg;
      if (count < NUM_TEETH) {
        moving_avg = 0;
      } else {
        const uint32_t avg = speed->big_sum[wheel] / SUM_ALL_TEETH;
        moving_avg = avg;
      }
      const bool timeout =
        speed->last_updated[wheel] + WHEEL_SPEED_TIMEOUT_MS < msTicks;
      speed->wheel_stopped[wheel] = timeout || count == 0;
      speed->disregard[wheel] = timeout;

      // Save value
      uint32_t *ptr;
      if (wheel == LEFT) {
        ptr = &speed->front_left_wheel_speed;
      } else if (wheel == RIGHT) {
        ptr = &speed->front_right_wheel_speed;
      } else continue;
      if (speed->wheel_stopped[wheel]) {
        *ptr = 0;
        continue;
      }
      if (count < NUM_TEETH) {
        *ptr = click_time_to_mRPM(speed->last_tick[wheel][idx]);
      } else {
        *ptr = click_time_to_mRPM(moving_avg);
      }
    }
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

void Input_handle_interrupt(Speed_Input_T *speed, uint32_t msTicks, uint32_t curr_tick, Wheel_T wheel) {
  if (speed->disregard[wheel]) {
    speed->num_ticks[wheel] = 0;
    speed->big_sum[wheel] = 0;
    speed->little_sum[wheel] = 0;
    speed->last_updated[wheel] = msTicks;
    return;
  }

  const uint32_t count = speed->num_ticks[wheel];
  const uint8_t idx = count % NUM_TEETH;
  const uint32_t this_tooth_last_rev =
    count < NUM_TEETH ? 0 : speed->last_tick[wheel][idx];

  // Register tick
  speed->last_tick[wheel][idx] = curr_tick;
  speed->num_ticks[wheel]++;

  // Update big sum
  speed->big_sum[wheel] += NUM_TEETH * curr_tick;
  speed->big_sum[wheel] -= speed->little_sum[wheel];

  // Update little sum
  speed->little_sum[wheel] += curr_tick;
  speed->little_sum[wheel] -= this_tooth_last_rev;

  // Update timestamp
  speed->last_updated[wheel] = msTicks;
}
