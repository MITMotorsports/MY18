#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "CANlib.h"

#define ACCEL_LOG_LENGTH 10
#define BRAKE_LOG_LENGTH 10

// Some wheel speed stuff
#define NUM_TEETH 23
#define SUM_ALL_TEETH (NUM_TEETH * (NUM_TEETH + 1) / 2)
#define CYCLES_PER_MICROSECOND 48

typedef struct {
  bool accel_1_under;
  bool accel_1_over;
  bool accel_2_under;
  bool accel_2_over;
  bool brake_1_under;
  bool brake_1_over;
  bool brake_2_under;
  bool brake_2_over;
} ADC_Errors_T;

typedef struct {
  // Raw accel values are read straight off of the adc
  uint16_t accel_1_raws[ACCEL_LOG_LENGTH];
  uint16_t accel_2_raws[ACCEL_LOG_LENGTH];
  // These accel values have been scaled to between 0 and 1000
  uint16_t accel_1;
  uint16_t accel_2;

  uint16_t brake_1_raws[BRAKE_LOG_LENGTH];
  uint16_t brake_2_raws[BRAKE_LOG_LENGTH];

  uint16_t brake_1;
  uint16_t brake_2;

  uint16_t steering_pot;

  uint32_t last_updated;

  ADC_Errors_T *errors;
} ADC_Input_T;

typedef enum {
  LEFT_32,
  RIGHT_32,
  LEFT_16,
  RIGHT_16,
  NUM_WHEELS
} Wheel_T;

typedef struct {
  // Various arrays/numbers to keep track of timing
  volatile uint32_t num_ticks[NUM_WHEELS];

  // integers in [0:4294967296] representing the number of clock cycles between
  // ticks from wheel speed sensors
  volatile uint32_t last_tick[NUM_WHEELS][NUM_TEETH];
  volatile uint64_t big_sum[NUM_WHEELS];

  // Total number of ticks the wheel has had in the past cycle
  // (i.e., the latest value for each tooth summed over the wheel)
  volatile uint64_t little_sum[NUM_WHEELS];

  // Time when interrupt was last called for each wheel
  uint32_t last_updated[NUM_WHEELS];

  volatile bool disregard[NUM_WHEELS];
  uint32_t last_speed_read_ms;
  bool wheel_stopped[NUM_WHEELS];

  // Actual values
  // Right is WHEEL2, left is WHEEL1
  uint32_t can_node_left_32b_wheel_speed;
  uint32_t can_node_left_16b_wheel_speed;
  uint32_t can_node_right_32b_wheel_speed;
  uint32_t can_node_right_16b_wheel_speed;
} Speed_Input_T;

typedef struct {
  ADC_Input_T *adc;
  Speed_Input_T *speed;
  uint32_t msTicks;
} Input_T;

#endif // TYPES_H
