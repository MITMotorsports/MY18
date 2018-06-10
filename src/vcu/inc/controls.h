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
// After wheel speed crosses this threshold, start using slip controller
#define LC_WS_THRESH 45000
#define LC_cGR 347 // Gear ratio times 100
#define LC_ACCEL_BEGIN 950 // 95%
#define LC_ACCEL_RELEASE 50 // 5%
#define LC_BRAKE_BEGIN 100 // We want a lower threshold

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

#define FRONT_BRAKE brake_1
#define REAR_BRAKE brake_2

typedef struct {
  bool using_regen;
  bool using_launch_control;
  uint16_t cBB_ef; // Electric front brake bias * 100
  int32_t slip_ratio;
} Controls_Settings_T;

typedef enum {
  BEFORE,
  SPEEDING_UP,
  SPEED_CONTROLLER,
  ZERO_TORQUE,
  DONE,
} Launch_Control_State_T;

extern Controls_Settings_T control_settings;

// PRIVATE FUNCTIONS
static int16_t get_torque(void);
static int32_t get_regen_torque(void);
static int32_t get_launch_control_speed(uint32_t front_wheel_speed);

// INTERACTION FUNCTIONS
void           enable_controls(void);
void           disable_controls(void);
bool           get_controls_enabled(void);

void           execute_controls(void);

#endif // ifndef __TORQUE_CALC
