#include "state.h"

static VCU_STATE_T currentState;

GateFaults_T gate_faults;
Heartbeats_T heartbeats;
Contactors_T contactors;
Conflicts_T  conflicts;

MCVoltage_T mc_voltage;
Voltages_T  voltages;
Pedalbox_T  pedalbox;
Buttons_T   buttons;

void init_vcu_state( void ) {
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
  set_vcu_state(VCU_STATE_LV_ONLY);
}

void set_vcu_state(VCU_STATE_T newState) {
  switch (currentState) {
  case VCU_STATE_LV_ONLY:
    initLVOnly();
    break;

  case VCU_STATE_PRECHARGING:
    initPrecharge();
    break;

  case VCU_STATE_CHARGE_FAULT:
    initChargeFault();
    break;

  case VCU_STATE_READY_TO_DRIVE:
    initReadyToDrive();
    break;

  case VCU_STATE_DRIVING:
    initDriving();
    break;

  case VCU_STATE_CONTACTOR_FAULT:
    initContactorFault();
    break;

	case VCU_STATE_HEARTBEAT_FAULT:
		Error_Handler("Heartbeat fault state.");
		return;

  default:
    printf("\r\n[WARNING]: INVALID CAR MODE CHANGE. %d\r\n", currentState);
    Error_Handler("Inside set_vcu_state.");
    return;
  }

  currentState = newState;
}

const inline VCU_STATE_T current_vcu_state() {
  return currentState;
}
