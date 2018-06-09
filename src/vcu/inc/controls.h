#ifndef __TORQUE_CALC
#define __TORQUE_CALC

#include "stm32f2xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#include "can_handles.h"
#include "fault_pedalbox.h"

// TODO: Extract MAX_TORQUE from RMS EEPROM (over CAN?)
#define MAX_TORQUE    2400 // unit: dNm
#define MAX_ACCEL_VAL 1000
#define MIN_ACCEL_VAL 0

// Launch control contants
#define GEAR_RATIO 3
#define LC_TIME_OFFSET 10
#define LC_TARGET_SLIP_RATIO 112 // Target ratio times 100
#define LC_SLEW_RATE_CAP 10
#define LC_KP 1000
#define LC_KI 1

// RG = regen
#define RG_MOTOR_SPEED_THRESH 250            // RPM
#define RG_CAR_SPEED_THRESH 5                // kph
#define RG_REAR_BRAKE_THRESH 827370          // Pa
#define RG_FRONT_BRAKE_THRESH 827370         // Pa
#define RG_BATTERY_VOLTAGE_MAX_THRESH 299000 // mV
#define RG_TORQUE_COMMAND_MAX 1000           // 10 * Nm

// K = mu_padrotor * 2 * r_rotoreffective * A_Cf / GR
// See equation 13 in Luis's thesis
// With plugged in constants:
// K = 0.35 * 2 * 0.0863 * 0.000792 / (52/15) = 1.38013615e-5
#define RG_10_7_K 138 // K * 10**7

#define REGEN true // Needs to eventually be set by dash
#define RG_cBB_ef 50  // Needs to eventually be set by dash. Also, is really BB_e_f * 100 (hence cBB_e_f)
#define FRONT_BRAKE brake_1
#define REAR_BRAKE brake_2


// PRIVATE FUNCTIONS
static int16_t get_torque(void);
static int32_t get_regen_torque(void);

// INTERACTION FUNCTIONS
void           enable_controls(void);
void           disable_controls(void);
bool           get_controls_enabled(void);

void           execute_controls(void);

#endif // ifndef __TORQUE_CALC
