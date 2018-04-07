#ifndef __TORQUE_CALC
#define __TORQUE_CALC

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "fault_pedalbox.h"

// TODO: Extract MAX_TORQUE from RMS EEPROM (over CAN?)
#define MAX_TORQUE    1600 // unit: dNm
#define MAX_ACCEL_VAL 1000
#define MIN_ACCEL_VAL 0

uint16_t calcTorque(uint16_t accel);

#endif // ifndef __TORQUE_CALC
