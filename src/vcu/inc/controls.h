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

// RG = regen
#define RG_MOTOR_SPEED_THRESH 250            // RPM
#define RG_CAR_SPEED_THRESH 5                // kph
#define RG_REAR_BRAKE_THRESH 827370          // Pa
#define RG_FRONT_BRAKE_THRESH 827370         // Pa
#define RG_BATTERY_VOLTAGE_MAX_THRESH 299000 // mV
#define RG_TORQUE_COMMAND_MAX 2000           // 10 * Nm

// K = mu_padrotor * 2 * r_rotoreffective * A_Cf / GR
// See equation 13 in Luis's thesis
// With plugged in constants:
// K = 0.35 * 2 * 0.0863 * 0.000792 / (52/15) = 1.38013615e-5
#define RG_10_7_K 138 // K * 10**7

#define FRONT_BRAKE brake_1
#define REAR_BRAKE brake_2

// TL = temp limiting
#define MAX_TEMP 60 // deciCelsius

// VL = (under)voltage limiting
#define MIN_VOLTAGE 250

extern can0_VCUControlsParams_T control_settings;

// PRIVATE FUNCTIONS
static int16_t get_torque(void);
static int32_t get_regen_torque(void);
static int16_t get_temp_limited_torque(int16_t pedal_torque);
static int16_t get_voltage_limited_torque(int16_t pedal_torque);

// INTERACTION FUNCTIONS
void enable_controls(void);
void disable_controls(void);
bool get_controls_enabled(void);
void execute_controls(void);

#endif // ifndef __TORQUE_CALC
