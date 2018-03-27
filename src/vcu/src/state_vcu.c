#include "state.h"

static VCU_STATE_T currentState;

MCVoltage_T mc_voltage = {};
Voltages_T  voltages = {};
Pedalbox_T  pedalbox = {};
Buttons_T   buttons = {};

void init_vcu_state(void) {
  // BOARD HEARTBEATS
  heartbeats.fcn = HAL_GetTick();
  heartbeats.bms = HAL_GetTick();
  heartbeats.mc  = HAL_GetTick();

  // BRAKE AND THROTTLE
  pedalbox.accel_1 = 0;
  pedalbox.accel_2 = 0;
  pedalbox.brake_1 = 0;
  pedalbox.brake_2 = 0;

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

VCU_STATE_T set_vcu_state(VCU_STATE_T newState) {
  switch (currentState) {
  case VCU_STATE_LV_ONLY:
    initLVOnly();
    break;

  case VCU_STATE_PRECHARGING:
    initPrecharge();
    break;

  case VCU_STATE_READY_TO_DRIVE:
    initReadyToDrive();
    break;

  case VCU_STATE_DRIVING:
    initDriving();
    break;

  default:
    // TODO: Add recoverer.
    Error_Handler("Inside set_vcu_state.");
    return;
  }

  currentState = newState;
}

VCU_STATE_T current_vcu_state(void) {
  return currentState;
}
