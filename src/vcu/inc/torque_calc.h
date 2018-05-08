#ifndef __TORQUE_CALC
#define __TORQUE_CALC

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "fault_pedalbox.h"

// TODO: Extract MAX_TORQUE from RMS EEPROM (over CAN?)
#define MAX_TORQUE    2400 // unit: dNm
#define MAX_ACCEL_VAL 1000
#define MIN_ACCEL_VAL 0

// Power limit constants. All in W
// Threshold at which we start clipping torque commands
#define PL_CLIP_THRESHOLD 300
// Threshold at which we stop sending torque commands
#define PL_ABS_THRESHOLD 400

uint16_t calcTorque(uint16_t accel);

#endif // ifndef __TORQUE_CALC
