#include "torque_calc.h"
#define HUMAN_READABLE true

uint16_t calcTorque(uint16_t accel) {
  static uint16_t last_torque = 0;
  static uint32_t LC_error_sum = 0;
  static int32_t PL_I_sum = 0;

  if (accel < PEDALBOX_ACCEL_RELEASE) return 0;

  uint16_t raw_torque = MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) / (MAX_ACCEL_VAL - PEDALBOX_ACCEL_RELEASE);

  // Launch control
  /*uint16_t launch_control_torque;
  if (true) {
    // If we are in the first few miliseconds, send max torque
    if (msTicks < LC_TIME_OFFSET) {
      if (raw_torque - last_torque > LC_SLEW_RATE_CAP) {
        launch_control_torque = last_torque + LC_SLEW_RATE_CAP
      } else launch_control_torque = raw_torque;
    } else {
      // Calculate setpoint
      uint16_t setpoint = mc_readings.speed * LC_TARGET_SLIP_RATIO;

      // Update PI controller
      uint16_t error =
    }
  } else {
    launch_control_torque = raw_torque;
  }*/



  // Power limit
  uint32_t power_limited_torque;
  int32_t error = cs_readings.power - PL_THRESHOLD;
  printf("i DELTA: %d", error * can0_MCCommand_period);
  PL_I_sum += error * can0_MCCommand_period;

  // Limit PL_I_sum
  if (PL_I_sum < 0) PL_I_sum = 0;
  if (PL_I_sum > PL_I_CAP) PL_I_sum = PL_I_CAP;

  if (error < 0) error = 0;

  // Speed starts negative becuase of direction
  int32_t mc_speed = mc_readings.speed * -1;

  uint32_t omega = mc_speed * (2 * 314)/ (60*100);

  // Multiplying by ten to improve accuracy and convert units
  uint32_t P_torque = 10 * error  * PL_KP;
  uint32_t I_torque = PL_KI * PL_I_sum;

  // PL_I sum is integrated with respect to ms
  // Both need to be divided by omega, but only PI needs to be dvided by 1000,
  // so multiply the P portion by 1000
  uint32_t torque_offset = (P_torque * 1000 + I_torque) / (1000 * omega);
  uint32_t PI_torque;
  if (torque_offset > raw_torque) {
    PI_torque = 0;
  } else {
    PI_torque = raw_torque - torque_offset;
  }

  // Mechanical limit
  if (omega != 0 && PI_torque > PL_THRESHOLD / omega) {
    power_limited_torque = PL_THRESHOLD / omega;
  } else {
    power_limited_torque = PI_torque;
  }

  if (HUMAN_READABLE) {
    printf("POWER: %d, RAW TORQUE: %d, TORQUE OFFSET: %d, COMMANDED TORQUE: %d, omega: %d, motor speed: %d, error: %d, I_torque: %d, I-sum:%d\r\n", cs_readings.power, raw_torque, torque_offset, power_limited_torque, omega, mc_speed, error, I_torque, PL_I_sum);
  } else {
    printf("power,%d,%d\r\n", cs_readings.power, HAL_GetTick());
    printf("raw_torque,%d,%d\r\n", raw_torque, HAL_GetTick());
    printf("torque_offset,%d,%d\r\n", torque_offset, HAL_GetTick());
    printf("power_limited_torque,%d,%d\r\n", power_limited_torque, HAL_GetTick());
    printf("omega,%d,%d\r\n",omega, HAL_GetTick());
    printf("motor_speed,%d,%d\r\n",mc_speed, HAL_GetTick());
    printf("error,%d,%d\r\n",error, HAL_GetTick());
  }

  pl.raw_torque = raw_torque;
  pl.power_limited_torque = power_limited_torque;
  pl.omega = omega;
  pl.error = error;
  pl.PI_torque = PI_torque;
  pl.torque_offset = torque_offset;
  pl.I_sum = PL_I_sum;
  pl.P_torque = P_torque;
  pl.I_torque = I_torque;

  return power_limited_torque;
}
