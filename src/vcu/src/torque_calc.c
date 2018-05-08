#include "torque_calc.h"

uint16_t calcTorque(uint16_t accel) {
  if (accel < PEDALBOX_ACCEL_RELEASE) return 0;

  uint16_t raw_torque = MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) / (MAX_ACCEL_VAL - PEDALBOX_ACCEL_RELEASE);

  uint16_t power_limited_torque;
  // Power limit
  if (cs_readings.power < PL_CLIP_THRESHOLD) {
    power_limited_torque = raw_torque;
  } else if (cs_readings.power < PL_ABS_THRESHOLD) {
    power_limited_torque = (PL_ABS_THRESHOLD - cs_readings.power) * raw_torque / (PL_ABS_THRESHOLD - PL_CLIP_THRESHOLD);
  } else {
    power_limited_torque = 0;
  }

  printf("power,%d,%d\r\n", cs_readings.power, HAL_GetTick());
  printf("raw_torque,%d,%d\r\n", raw_torque, HAL_GetTick());
  printf("power_limited_torque,%d,%d\r\n", power_limited_torque, HAL_GetTick());

  return power_limited_torque;
}
