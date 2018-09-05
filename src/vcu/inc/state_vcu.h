#ifndef __STATE_VCU_H
#define __STATE_VCU_H

#include <stdbool.h>
#include <stdint.h>

#include "state_vcu_root.h"
#include "state_vcu_lv.h"
#include "state_vcu_precharge.h"
#include "state_vcu_rtd.h"
#include "state_vcu_driving.h"

#define TEMP_LOG_LENGTH 200

// DEFINITION OF CAR STATES
typedef enum {
  VCU_STATE_ROOT = 0,
  VCU_STATE_LV = 1,
  VCU_STATE_PRECHARGING = 2,
  VCU_STATE_RTD = 3,
  VCU_STATE_DRIVING = 4,
} VCU_STATE_T;


// DEFINITION OF CONTAINER TYPES
typedef struct {
  Voltage_T bus;
  Voltage_T pack;
  Voltage_T precharge_target;
} Voltages_T;

typedef struct {
  int16_t V_bus;
  int16_t V_out;
  int16_t V_VAB_Vd;
  int16_t V_VBC_Vq;
  int16_t speed;
  int16_t torque_feedback;

  bool can_fault;
} MCReadings_T;

typedef struct {
  int32_t V_bus;
  int32_t power;
} CSReadings_T;

typedef struct {
  bool RTD;
  bool MasterReset;
  bool DriverReset;
} Buttons_T;

typedef struct {
  int16_t north;
  int16_t east;
} IMUVelocity_T;

typedef struct {
  uint16_t cell_min_cV;
  uint16_t temp_log[TEMP_LOG_LENGTH];
} Cell_Readings_T;

typedef struct {
  uint32_t front_left_32b_wheel_speed;
  uint32_t front_left_16b_wheel_speed;
  uint32_t front_right_32b_wheel_speed;
  uint32_t front_right_16b_wheel_speed;
} Wheel_Speeds_T;

// GLOBAL CONTAINER DEFINITIONS
extern volatile Voltages_T  voltages;
extern volatile Buttons_T   buttons;

extern volatile MCReadings_T mc_readings;
extern volatile CSReadings_T cs_readings;
extern volatile IMUVelocity_T imu_velocity;
extern volatile Wheel_Speeds_T wheel_speeds;
extern volatile Cell_Readings_T cell_readings;
extern can0_VCUControlsMonitoring_T controls_monitoring;

// INTERACTION FUNCTIONS
void        init_vcu_state(void);
VCU_STATE_T set_vcu_state(VCU_STATE_T newState);
void        advance_vcu_state(void);

VCU_STATE_T get_vcu_state(void);


#endif // ifndef __STATE_VCU_H
