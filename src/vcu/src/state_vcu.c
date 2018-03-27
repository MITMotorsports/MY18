#include "state.h"

static VCU_STATE_T currentState;

volatile MCVoltage_T mc_voltage = {};
volatile Voltages_T  voltages = {};
volatile Pedalbox_T  pedalbox = {};
volatile Buttons_T   buttons = {};

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

void advance_vcu_state(void) {
  switch (current_vcu_state()) {
  case VCU_STATE_LV_ONLY:
    loopLVOnly();
    break;

  case VCU_STATE_PRECHARGING:
    loopPrecharge();
    break;

  case VCU_STATE_CHARGE_FAULT:
    loopChargeFault();
    break;

  case VCU_STATE_READY_TO_DRIVE:
    loopReadyToDrive();
    break;

  case VCU_STATE_DRIVING:
    loopDriving();
    break;

  case VCU_STATE_CONTACTOR_FAULT:
    loopContactorFault();
    break;

  case VCU_STATE_HEARTBEAT_FAULT:
    loopHeartbeatFault();
    break;

  default:
    Error_Handler("Inside loopVCU.");
    break;
  }
}

VCU_STATE_T current_vcu_state(void) {
  return currentState;
}
