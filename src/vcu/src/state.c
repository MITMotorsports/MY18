#include "state.h"
#include "lv_only.h"
#include "precharge.h"
#include "charge_fault.h"
#include "ready_to_drive.h"
#include "driving.h"
#include "contactor_fault.h"

GateFaults_T gate_faults;
Heartbeats_T heartbeats;
Contactors_T contactors;
Conflicts_T  conflicts;

MCVoltage_T mc_voltage;
Voltages_T  voltages;
Pedalbox_T  pedalbox;
Buttons_T   buttons;

void initVCUState(void) {
  // BOARD HEARTBEATS
  heartbeats.frontCanNode = HAL_GetTick();
  heartbeats.bms          = HAL_GetTick();
  heartbeats.mc           = HAL_GetTick();

  // BRAKE AND THROTTLE
  pedalbox.accel_1 = 0;
  pedalbox.accel_2 = 0;
  pedalbox.brake_1 = 0;
  pedalbox.brake_2 = 0;

  // IMPLAUSIBILITY AND BRAKE CHECK
  conflicts.has_brake_throttle_conflict = false;
  conflicts.observed_implausibility     = false;
  conflicts.actual_implausibility       = false;
  conflicts.implausibility_ticks        = 0;

  // DC BUS VOLTAGE OF MOTOR CONTROLLER
  mc_voltage.busVoltage     = 0;
  mc_voltage.outVoltage     = 0;
  mc_voltage.VAB_Vd_Voltage = 0;
  mc_voltage.VBC_Vq_Voltage = 0;

  // BMS PACK VOLTAGE
  voltages.packVoltage = 0;

  // BUTTON PRESSES
  buttons.RTD          = 0;
  buttons.DriverReset  = 0;
  buttons.ScrollSelect = 0;

  // CAR Mode
  changeCarMode(CAR_STATE_LV_ONLY);
}

void changeCarMode(CAR_STATE_T newState) {
  switch (carMode) {
  case CAR_STATE_LV_ONLY:
    carMode = newState;
    initLVOnly();
    break;

  case CAR_STATE_PRECHARGING:
    carMode = newState;
    initPrecharge();
    break;

  case CAR_STATE_CHARGE_FAULT:
    carMode = newState;
    initChargeFault();
    break;

  case CAR_STATE_READY_TO_DRIVE:
    carMode = newState;
    initReadyToDrive();
    break;

  case CAR_STATE_DRIVING:
    carMode = newState;
    initDriving();
    break;

  case CAR_STATE_CONTACTOR_FAULT:
    carMode = newState;
    initContactorFault();
    break;

  default:
    printf("\r\n[WARNING]: INVALID CAR MODE CHANGE. %d\r\n", carMode);
    break;
  }
}
