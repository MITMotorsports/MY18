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
  VCU_STATE_ROOT,
  VCU_STATE_LV,
  VCU_STATE_PRECHARGING,
  VCU_STATE_RTD,
  VCU_STATE_DRIVING,
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
} MCReadings_T;

typedef struct {
  int16_t V_bus;
} CSReadings_T;

typedef struct {
  int16_t accel_1;
  int16_t accel_2;
  int16_t brake_1;
  int16_t brake_2;
} Pedalbox_T;

typedef struct {
  bool RTD;
  bool MasterReset;
  bool DriverReset;
  bool ScrollSelect;
} Buttons_T;


// GLOBAL CONTAINER DEFINITIONS
extern volatile Voltages_T  voltages;
extern volatile Pedalbox_T  pedalbox;
extern volatile Buttons_T   buttons;

extern volatile MCReadings_T mc_readings;
extern volatile CSReadings_T cs_readings;


// INTERACTION FUNCTIONS
void        init_vcu_state(void);
VCU_STATE_T set_vcu_state(VCU_STATE_T newState);
void        advance_vcu_state(void);

VCU_STATE_T get_vcu_state(void);


#endif // ifndef __STATE_VCU_H
