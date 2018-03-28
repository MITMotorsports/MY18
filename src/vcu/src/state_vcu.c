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
  voltages.pack = 0;

  // BUTTON PRESSES
  buttons.RTD          = 0;
  buttons.MasterReset  = 0;
  buttons.DriverReset  = 0;
  buttons.ScrollSelect = 0;

  // CAR Mode
  set_vcu_state(VCU_STATE_ROOT);
}

VCU_STATE_T set_vcu_state(VCU_STATE_T newState) {
  switch (newState) {
  case VCU_STATE_ROOT:
    enter_vcu_state_root();
    break;

  case VCU_STATE_LV_ONLY:
    enter_vcu_state_lv();
    break;

  case VCU_STATE_PRECHARGING:
    enter_vcu_state_precharge();
    break;

  case VCU_STATE_READY_TO_DRIVE:
    enter_vcu_state_rtd();
    break;

  case VCU_STATE_DRIVING:
    enter_vcu_state_driving();
    break;

  default:
    // TODO: Add recoverer.
    Error_Handler("Inside set_vcu_state.");
    return;
  }

  currentState = newState;
}

void advance_vcu_state(void) {
  switch (get_vcu_state()) {
  case VCU_STATE_ROOT:
    update_vcu_state_root();
    break;

  case VCU_STATE_LV_ONLY:
    update_vcu_state_lv();
    break;

  case VCU_STATE_PRECHARGING:
    update_vcu_state_precharge();
    break;

  case VCU_STATE_PRECHARGE_FAULT:
    break;

  case VCU_STATE_READY_TO_DRIVE:
    update_vcu_state_rtd();
    break;

  case VCU_STATE_DRIVING:
    update_vcu_state_driving();
    break;

  default:
    Error_Handler("Inside loopVCU.");
    break;
  }
}

VCU_STATE_T get_vcu_state(void) {
  return currentState;
}
