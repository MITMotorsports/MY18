#include "state.h"

static VCU_STATE_T currentState;

volatile Voltages_T   voltages    = {};
volatile Buttons_T    buttons     = {};
volatile MCReadings_T mc_readings = {};
volatile CSReadings_T cs_readings = {};
volatile IMUVelocity_T imu_velocity = {};
volatile Wheel_Speeds_T wheel_speeds = {};
volatile Cell_Readings_T cell_readings = {};
can0_VCUControlsMonitoring_T controls_monitoring;

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

  // MOTOR CONTROLLER READINGS
  mc_readings.V_bus    = 0;
  mc_readings.V_out    = 0;
  mc_readings.V_VAB_Vd = 0;
  mc_readings.V_VBC_Vq = 0;
  mc_readings.speed = 0;

  // CURRENT SENSOR READINGS
  cs_readings.V_bus = 0;

  // BMS PACK VOLTAGE
  voltages.pack = 0;

  // BUTTON PRESSES
  buttons.RTD          = 0;
  buttons.MasterReset  = 0;
  buttons.DriverReset  = 0;

  for (uint8_t i = 0; i < TEMP_LOG_LENGTH; i++) {
    cell_readings.temp_log[i] = 0;
  }

  controls_monitoring.raw_torque = 0;
  controls_monitoring.vl_gain = 0;
  controls_monitoring.tl_gain = 0;
  controls_monitoring.filtered_temp = 0;

  init_controls_defaults();

  // CAR Mode
  set_vcu_state(VCU_STATE_ROOT);
  // TODO: Remove this skip to RTD.
  // set_vcu_state(VCU_STATE_DRIVING);
}

VCU_STATE_T set_vcu_state(VCU_STATE_T newState) {
  switch (newState) {
  case VCU_STATE_ROOT:
    enter_vcu_state_root();
    break;

  case VCU_STATE_LV:
    enter_vcu_state_lv();
    break;

  case VCU_STATE_PRECHARGING:
    enter_vcu_state_precharge();
    break;

  case VCU_STATE_RTD:
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

  case VCU_STATE_LV:
    update_vcu_state_lv();
    break;

  case VCU_STATE_PRECHARGING:
    update_vcu_state_precharge();
    break;

  case VCU_STATE_RTD:
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
