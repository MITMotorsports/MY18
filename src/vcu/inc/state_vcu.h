#ifndef __STATE_VCU_H
#define __STATE_VCU_H

#include <stdbool.h>
#include <stdint.h>

#include "state_vcu_root.h"
#include "state_vcu_lv.h"
#include "state_vcu_precharge.h"
#include "state_vcu_rtd.h"
#include "state_vcu_driving.h"


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
} MCReadings_T;

typedef struct {
  int16_t V_bus;
  int32_t power;
} CSReadings_T;

typedef struct {
  bool RTD;
  bool MasterReset;
  bool DriverReset;
  bool ScrollSelect;
} Buttons_T;

typedef struct {
  uint32_t raw_torque;
  uint32_t power_limited_torque;
  uint32_t omega;
  int32_t error;
  uint32_t PI_torque;
  uint32_t torque_offset;
  int32_t I_sum;
  uint32_t P_torque;
  uint32_t I_torque;
} PL_T;

// GLOBAL CONTAINER DEFINITIONS
extern volatile Voltages_T  voltages;
extern volatile Buttons_T   buttons;

extern volatile MCReadings_T mc_readings;
extern volatile CSReadings_T cs_readings;
extern volatile PL_T pl;

// INTERACTION FUNCTIONS
void        init_vcu_state(void);
VCU_STATE_T set_vcu_state(VCU_STATE_T newState);
void        advance_vcu_state(void);

VCU_STATE_T get_vcu_state(void);


#endif // ifndef __STATE_VCU_H
