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
  bool bms_fault;
  bool bpd_fault;
  bool imd_fault;
  bool sdn_fault;
  bool esd_fault;
  bool tsms_fault;
} GateFaults_T;

typedef struct {
  bool L_contactor_closed;
  bool H_contactor_closed;
  bool L_contactor_welded;
  bool H_contactor_welded;
} Contactors_T;

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
  uint32_t frontCanNode;
  uint32_t bms;
  uint32_t mc;
} Heartbeats_T;

typedef struct {
  int16_t accel_1;
  int16_t accel_2;
  int16_t brake_1;
  int16_t brake_2;
} Pedalbox_T;

typedef struct {
  bool     has_brake_throttle_conflict;
  bool     observed_implausibility;
  bool     actual_implausibility;
  uint32_t implausibility_ticks;
} Conflicts_T;

typedef struct {
  bool RTD;
  bool DriverReset;
  bool ScrollSelect;
} Buttons_T;



// GLOBAL CONTAINER DEFINITIONS
extern GateFaults_T gate_faults;
extern Heartbeats_T heartbeats;
extern Contactors_T contactors;
extern Conflicts_T  conflicts;

extern MCVoltage_T mc_voltage;
extern Voltages_T  voltages;
extern Pedalbox_T  pedalbox;
extern Buttons_T   buttons;



// INTERACTION FUNCTIONS
void init_vcu_state();
void change_vcu_state();
void update_vcu_state();

const VCU_STATE_T current_vcu_state();



#endif // ifndef __STATE_VCU_H
