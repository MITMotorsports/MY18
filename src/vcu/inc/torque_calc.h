#ifndef __TORQUE_CALC
#define __TORQUE_CALC

#include "stdio.h"
#include "stm32f2xx_hal.h"

#define MAX_TORQUE              5 // N-m
#define TORQUE_SCALE_FACTOR     100
#define MAX_SENSOR_VAL          1024
#define MIN_SENSOR_VAL          0

uint16_t calcTorque(uint16_t accel1, uint16_t accel2);

#endif
