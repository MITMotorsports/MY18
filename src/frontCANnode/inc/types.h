#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "Can_Library.h"

typedef struct {
  uint16_t accel_1;
  uint16_t accel_2;
  uint16_t brake_1;
  uint16_t brake_2;

  uint32_t last_updated;
} ADC_Input_T;

typedef struct {
  uint32_t front_right_wheel_speed;
  uint32_t front_left_wheel_speed;
} Speed_Input_T;

// Based off of subsribe fields in CAN spec
typedef struct {
  Can_VcuToDashID_T limp_state; // For limp mode
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
  // TODO fill this out with torque calculation stuff
  // Torque based on throttle pedal
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
