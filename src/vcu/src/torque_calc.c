#include "torque_calc.h"

// uint16_t map(uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
//
// }

uint16_t calcTorque(uint16_t accel1, uint16_t accel2) {
  // Get the min value
  uint32_t accel = (accel1 < accel2) ? accel1 : accel2;

  accel = (accel < MIN_ACCEL_VAL) ? MIN_ACCEL_VAL : accel;
  accel = (accel > MAX_ACCEL_VAL) ? MAX_ACCEL_VAL : accel;

  uint16_t torque = MAX_TORQUE * accel / MAX_ACCEL_VAL;
  return torque;
}
