#include "torque_calc.h"

uint16_t calcTorque(uint16_t accel) {
  if (accel < PEDALBOX_ACCEL_RELEASE) return 0;

  uint16_t torque = MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) / MAX_ACCEL_VAL;
  return torque;
}
