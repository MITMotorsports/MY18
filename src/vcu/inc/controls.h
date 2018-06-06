#ifndef __TORQUE_CALC
#define __TORQUE_CALC

#include "stdio.h"
#include "stm32f2xx_hal.h"

#include "can_handles.h"
#include "fault_pedalbox.h"
#include "state_vcu.h"

// TODO: Extract MAX_TORQUE from RMS EEPROM (over CAN?)
#define MAX_TORQUE    2400 // unit: dNm
#define MAX_ACCEL_VAL 1000
#define MIN_ACCEL_VAL 0

// Power limit constants. All in W
// Threshold at which we start to adjust torque commands to limit power
#define PL_THRESHOLD 400
#define PL_KP 1
#define PL_KI 100
#define PL_I_CAP 1000000

// Launch control contants
#define GEAR_RATIO 3
#define LC_TIME_OFFSET 10
#define LC_TARGET_SLIP_RATIO 112 // Target ratio times 100
#define LC_SLEW_RATE_CAP 10
#define LC_KP 1000
#define LC_KI 1

// RG = regen
#define RG_MOTOR_SPEED_THRESH 250 // RPM
#define RG_CAR_SPEED_THRESH 5 // kph
#define RG_REAR_BRAKE_THRESH 827370 // Pa
#define RG_FRONT_BRAKE_THRESH 827370 // Pa
#define RG_BATTERY_VOLTAGE_MAX_THRESH 299000 // mV
#define RG_ACCEL_MAX_THRESH 5 // %
#define RG_TORQUE_COMMAND_MAX 100 // Nm
#define RG_K 1

// Need to eventually be set by dash
#define REGEN true
#define BB_ef 1


// PRIVATE FUNCTIONS
static int16_t get_torque(void);
static int16_t get_regen_torque(void);
static int16_t get_launch_control_torque(void);
static int16_t get_pwr_limited_torque(int16_t raw_torque);

// INTERACTION FUNCTIONS
void enable_controls(void);
void disable_controls(void);
void execute_controls(void);

#endif // ifndef __TORQUE_CALC
