#include "input.h"

extern Input_T input;
extern volatile uint32_t msTicks;

void update_adc(void);
void update_wheel_speed(void);
void update_can(void);


uint16_t transform1(uint32_t accel_1_raw);
uint16_t transform2(uint32_t accel_2_raw);

uint16_t adc_log[10];

void Input_fill_input() {
  update_adc();
  update_wheel_speed();
}

void update_adc() {
  ADC_Input_T *adc = input.adc;
  uint32_t next_updated = adc->last_updated + ADC_UPDATE_PERIOD_MS;


  if (next_updated < input.msTicks) {
    // Update accels
    for (int i = 0; i < ACCEL_LOG_LENGTH - 1; i++) {
      adc->accel_1_raws[i] = adc->accel_1_raws[i+1];
      adc->accel_2_raws[i] = adc->accel_2_raws[i+1];
    }
    adc->accel_1_raws[ACCEL_LOG_LENGTH-1] = ADC_Read(ACCEL_1_CHANNEL);
    adc->accel_2_raws[ACCEL_LOG_LENGTH-1] = ADC_Read(ACCEL_2_CHANNEL);
    uint32_t accel_1_sum = 0;
    uint32_t accel_2_sum = 0;
    for (int i = 0; i < ACCEL_LOG_LENGTH; i ++) {
      accel_1_sum += adc->accel_1_raws[i];
      accel_2_sum += adc->accel_2_raws[i];
    }
    adc->accel_1 = transform1(accel_1_sum / ACCEL_LOG_LENGTH);
    adc->accel_2 = transform2(accel_2_sum / ACCEL_LOG_LENGTH);

    // Update brakes
    for (int i = 0; i < BRAKE_LOG_LENGTH - 1; i++) {
      adc->brake_1_raws[i] = adc->brake_1_raws[i+1];
      adc->brake_2_raws[i] = adc->brake_2_raws[i+1];
    }
    adc->brake_1_raws[BRAKE_LOG_LENGTH-1] = ADC_Read(BRAKE_1_CHANNEL);
    adc->brake_2_raws[BRAKE_LOG_LENGTH-1] = ADC_Read(BRAKE_2_CHANNEL);
    uint32_t brake_1_sum = 0;
    uint32_t brake_2_sum = 0;
    for (int i = 0; i < BRAKE_LOG_LENGTH; i ++) {
      brake_1_sum += adc->brake_1_raws[i];
      brake_2_sum += adc->brake_2_raws[i];
    }
    adc->brake_1 = brake_1_sum / BRAKE_LOG_LENGTH;
    adc->brake_2 = brake_2_sum / BRAKE_LOG_LENGTH;

    // Update other sensors
    adc->steering_pot = ADC_Read(STEERING_CHANNEL);
    adc->last_updated = input.msTicks;
  }
}

uint16_t click_time_to_mRPM(uint32_t us_per_click) {
  if (us_per_click == 0) {
    return 0;
  }
  // Convert milliseconds per click to milli rpm
  const float us_per_rev = us_per_click * 1.0 * NUM_TEETH;

  const float s_per_rev = us_per_rev / MICROSECONDS_PER_SECOND_F;

  const float mrev_per_s = MILLIREVS_PER_REV_F / s_per_rev;

  const float mrev_per_min = mrev_per_s * SECONDS_PER_MINUTE;
  return (uint16_t)mrev_per_min;
}

void update_wheel_speed() {
  Speed_Input_T* speed = input.speed;
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
      uint16_t *ptr;
      switch (wheel) {
        case LEFT_A:
          ptr = &speed->front_left_A_wheel_speed;
          break;
        case LEFT_B:
          ptr = &speed->front_left_B_wheel_speed;
          break;
        case RIGHT_A:
          ptr = &speed->front_right_A_wheel_speed;
          break;
        case RIGHT_B:
          ptr = &speed->front_right_B_wheel_speed;
          break;
        default:
          continue;
      }

      if (count < NUM_TEETH) {
        *ptr = click_time_to_mRPM(speed->last_tick[wheel][idx]);
      } else {
        *ptr = click_time_to_mRPM(moving_avg);
      }

      *ptr = click_time_to_mRPM(speed->last_tick[wheel][idx]);
    }
  }
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
  // Scale to between 0 and ACCEL_SCALE_MAX
  return linear_transfer_fn(accel_1_raw, ACCEL_SCALE_MAX, ACCEL_1_LOWER_BOUND, ACCEL_1_UPPER_BOUND);
}

uint16_t transform2(uint32_t accel_2_raw) {
  // Scale to between 0 and ACCEL_SCALE_MAX
  return linear_transfer_fn(accel_2_raw, ACCEL_SCALE_MAX, ACCEL_2_LOWER_BOUND, ACCEL_2_UPPER_BOUND);
}

void Input_handle_interrupt(uint32_t msTicks, uint32_t curr_tick, Wheel_T wheel) {
  Speed_Input_T *speed = input.speed;
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
