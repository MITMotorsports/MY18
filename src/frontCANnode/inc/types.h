#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "Can_Library.h"

// Some wheel speed stuff
#define NUM_TEETH 23
#define SUM_ALL_TEETH (NUM_TEETH * (NUM_TEETH + 1) / 2)
#define CYCLES_PER_MICROSECOND 48

typedef struct {
  // Raw accel values are read straight off of the adc
  uint16_t accel_1_raw;
  uint16_t accel_2_raw;
  // These accel values have been scaled to between 0 and 1000
  uint16_t accel_1;
  uint16_t accel_2;

  uint16_t brake_1;
  uint16_t brake_2;

  uint16_t steering_pot;

  uint32_t last_updated;
} ADC_Input_T;

typedef enum {
  LEFT,
  RIGHT,
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
  uint32_t front_right_wheel_speed;
  uint32_t front_left_wheel_speed;
} Speed_Input_T;

// Should match subscribe fields in CAN spec
typedef struct {
  Can_VcuToDashID_T limp_state;
} Can_Input_T;

typedef struct {
  ADC_Input_T *adc;
  Speed_Input_T *speed;
  Can_Input_T *can_input;
  uint32_t msTicks;
} Input_T;


typedef struct {
  // implausibility_observed is true iff there is currently an implausibility
  bool implausibility_observed;

  // implausibility_time_ms is set to timestamp of the most recent time that
  // implausibility_observed switched from false to true
  uint32_t implausibility_time_ms;

  // implausibility_reported is true iff implausibility_observed has been true
  // for >100ms (EV2.3.5)
  bool implausibility_reported;
  bool has_brake_throttle_conflict;
} Rules_State_T;

typedef struct {
  // TODO fill this out with other stuff for torque calculations if needed
  // Torque based on accel pedal
  int16_t throttle_torque;
  // Torque that will actually be sent to the motor controller
  int16_t requested_torque;
} Torque_State_T;

typedef struct {
  uint32_t can_output_ms; // for FrontCanNodeOutput
  uint32_t can_wheel_speed_ms; // for FrontCanNodeWheelSpeed
  bool send_output_msg;
  bool send_wheel_speed_msg;
} Can_Output_State_T;

typedef struct {
  Rules_State_T *rules;
  Torque_State_T *torque;
  Can_Output_State_T *can_output;
} State_T;

#endif // TYPES_H
