#ifndef __STATE_VCU_H
#define __STATE_VCU_H

#include <stdbool.h>
#include <stdint.h>


// DEFINITION OF CAR STATES
typedef enum {
  VCU_STATE_LV_ONLY,
  VCU_STATE_PRECHARGING,
  VCU_STATE_CHARGE_FAULT,
  VCU_STATE_READY_TO_DRIVE,
  VCU_STATE_DRIVING,
  VCU_STATE_CONTACTOR_FAULT,
  VCU_STATE_HEARTBEAT_FAULT,
} VCU_STATE_T;


// DEFINITION OF CONTAINER TYPES
typedef struct {
  int16_t packVoltage;
  int32_t dc_bus_voltage;
} Voltages_T;

typedef struct {
  int16_t busVoltage;
  int16_t outVoltage;
  int16_t VAB_Vd_Voltage;
  int16_t VBC_Vq_Voltage;
} MCVoltage_T;

typedef struct {
  int16_t accel_1;
  int16_t accel_2;
  int16_t brake_1;
  int16_t brake_2;
} Pedalbox_T;

typedef struct {
  bool RTD;
  bool DriverReset;
  bool ScrollSelect;
} Buttons_T;


// GLOBAL CONTAINER DEFINITIONS
extern volatile MCVoltage_T mc_voltage;
extern volatile Voltages_T  voltages;
extern volatile Pedalbox_T  pedalbox;
extern volatile Buttons_T   buttons;


// INTERACTION FUNCTIONS
void        init_vcu_state(void);
VCU_STATE_T set_vcu_state(VCU_STATE_T newState);
void        advance_vcu_state(void);

VCU_STATE_T current_vcu_state(void);


#endif // ifndef __STATE_VCU_H
