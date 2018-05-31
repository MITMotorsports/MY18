#ifndef __TORQUE_CALC
#define __TORQUE_CALC

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "fault_pedalbox.h"
#include "state_vcu.h"

// TODO: Extract MAX_TORQUE from RMS EEPROM (over CAN?)
#define MAX_TORQUE    2400 // unit: dNm
#define MAX_ACCEL_VAL 1000
#define MIN_ACCEL_VAL 0

// Power limit constants. All in W
// Threshold at which we start to adjust torque commands to limit power
#define PL_THRESHOLD 2000
#define PL_KP 1
#define PL_KI 0
#define PL_I_CAP 2000

// Launch control contants
#define GEAR_RATIO 3
#define LC_TIME_OFFSET 10
#define LC_TARGET_SLIP_RATIO 112 // Target ratio times 100
#define LC_SLEW_RATE_CAP 10
#define LC_KP 1000
#define LC_KI 1

uint16_t calcTorque(uint16_t accel);

#endif // ifndef __TORQUE_CALC
