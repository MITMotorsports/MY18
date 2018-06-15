#ifndef _SPEED__H_
#define _SPEED__H_

#define NUM_TEETH 23
#define SUM_ALL_TEETH (NUM_TEETH * (NUM_TEETH + 1) / 2)
#define CYCLES_PER_MICROSECOND 48

typedef enum {
  LEFT_32,
  RIGHT_32,
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
  uint32_t can_node_right_32b_wheel_speed;
} Speed_Input_T;

#endif