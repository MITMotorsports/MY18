#include "torque_calc.h"

uint16_t calcTorque(uint16_t accel) {
  uint16_t torque = MAX_TORQUE * accel / MAX_ACCEL_VAL;
  return torque;
}
