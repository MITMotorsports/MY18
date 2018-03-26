#ifndef __STATE_H
#define __STATE_H

#include <stdbool.h>
#include <stdint.h>

// DEFINITION OF CAR MODES
typedef enum {
  CAR_STATE_LV_ONLY,
  CAR_STATE_PRECHARGING,
  CAR_STATE_CHARGE_FAULT,
  CAR_STATE_READY_TO_DRIVE,
  CAR_STATE_DRIVING,
  CAR_STATE_CONTACTOR_FAULT,
  CAR_STATE_HEARTBEAT_FAULT,
} CAR_STATE_T;

CAR_STATE_T carMode;

typedef struct
{
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

typedef struct
{
  int16_t busVoltage;
  int16_t outVoltage;
  int16_t VAB_Vd_Voltage;
  int16_t VBC_Vq_Voltage;
} MCVoltage_T;

typedef struct
{
  uint32_t frontCanNode;
  uint32_t bms;
  uint32_t mc;
} Heartbeats_T;

typedef struct
{
  int16_t accel_1;
  int16_t accel_2;
  int16_t brake_1;
  int16_t brake_2;
} Pedalbox_T;

typedef struct
{
  bool     has_brake_throttle_conflict;
  bool     observed_implausibility;
  bool     actual_implausibility;
  uint32_t implausibility_ticks;
} Conflicts_T;

typedef struct
{
  bool RTD;
  bool DriverReset;
  bool ScrollSelect;
} Buttons_T;

// ACTUAL OBJECTS
extern GateFaults_T gate_faults;
extern Heartbeats_T heartbeats;
extern Contactors_T contactors;
extern Conflicts_T  conflicts;

extern MCVoltage_T mc_voltage;
extern Voltages_T  voltages;
extern Pedalbox_T  pedalbox;
extern Buttons_T   buttons;

void initVCUState(void);

void changeCarMode(uint8_t newState);

#endif // ifndef __STATE_H
