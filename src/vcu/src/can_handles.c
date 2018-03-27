#include "can_handles.h"


void handleCAN(CAN_HandleTypeDef *CanHandle) {
  Frame frame;

  // TODO: Maybe use CanHandle to get last RxMsg here.

  lastRxMsgToFrame(&frame);

  can0_T msgForm;
  msgForm = identify_can0(&frame);

  switch (msgForm) {
  case can0_FrontCanNodeBrakeThrottle:
    handleBrakeThrottleMsg(&frame);
    break;

  case can0_BMSHeartbeat:
    handleBMSHeartbeatMsg(&frame);
    break;

  case can0_CellVoltages:
    handleCellVoltagesMsg(&frame);
    break;

  case can0_CurrentSensor_Voltage:
    handleCurrentSensorVoltageMsg(&frame);
    break;

  case can0_MCVoltage:
    handleMCVoltageMsg(&frame);
    break;

  default:
    break;
  }
}

void handleBrakeThrottleMsg(Frame *msg) {
  can0_FrontCanNodeBrakeThrottle_T unpacked_msg;

  unpack_can0_FrontCanNodeBrakeThrottle(msg, &unpacked_msg);

  pedalbox.accel_1 = unpacked_msg.accel_1;
  pedalbox.accel_2 = unpacked_msg.accel_2;
  pedalbox.brake_1 = unpacked_msg.brake_1;
  pedalbox.brake_2 = unpacked_msg.brake_2;

  heartbeats.fcn = HAL_GetTick();
}

void handleMCVoltageMsg(Frame *msg) {
  can0_MCVoltage_T unpacked_msg;

  unpack_can0_MCVoltage(msg, &unpacked_msg);

  mc_voltage.busVoltage     = unpacked_msg.busVoltage;
  mc_voltage.outVoltage     = unpacked_msg.outVoltage;
  mc_voltage.VAB_Vd_Voltage = unpacked_msg.VAB_Vd_Voltage;
  mc_voltage.VBC_Vq_Voltage = unpacked_msg.VBC_Vq_Voltage;

  heartbeats.mc = HAL_GetTick();
}

void handleBMSHeartbeatMsg(Frame *msg) {
  // can0_BMSHeartbeat_T unpacked_msg;
  // unpack_can0_BMSHeartbeat(msg, &unpacked_msg);

  // TODO: Make not manual.
  contactors.L_contactor_closed = (msg->data[0] & 2) != 0;
  contactors.H_contactor_closed = (msg->data[0] & 4) != 0;
  contactors.L_contactor_welded = (msg->data[0] & 8) != 0;
  contactors.H_contactor_welded = (msg->data[0] & 16) != 0;

  heartbeats.bms = HAL_GetTick();
}

void handleCurrentSensorVoltageMsg(Frame *msg) {
  can0_CurrentSensor_Voltage_T unpacked_msg;

  unpack_can0_CurrentSensor_Voltage(msg, &unpacked_msg);

  voltages.dc_bus_voltage = unpacked_msg.dc_bus_voltage;
}

void handleCellVoltagesMsg(Frame *msg) {
  can0_CellVoltages_T unpacked_msg;

  unpack_can0_CellVoltages(msg, &unpacked_msg);

  // So we take the cell voltage of the minimum cell and use that
  // to estimate the lower bound on the back voltage
  // (12 per cell, 6 cells, millivolts)
  voltages.packVoltage = unpacked_msg.min * 12 * 6 / 1000;
}

void handleButtonRequest(Frame *msg) {
  can0_ButtonRequest_T unpacked_msg;

  unpack_can0_ButtonRequest(msg, &unpacked_msg);

  buttons.RTD          = unpacked_msg.RTD;
  buttons.DriverReset  = unpacked_msg.DriverReset;
  buttons.ScrollSelect = unpacked_msg.ScrollSelect;
}

// void sendDashMsg() {
//   can0_VcuToDash_T msg;
//
//   // msg.rtd_light_on = //is this right? (carMode == VCU_STATE_READY_TO_DRIVE
// ||
//   // carMode == VCU_STATE_DRIVING);
//   // msg.ams_light_on = //what;
//   // msg.imd_light_on = //what;
//   // msg.hv_light_on = //what;
//   // msg.traction_control = //what;
//   // msg.limp_mode_on = //what;
//   // msg.lv_warning_on = //what;
//   // msg.active_aero_on = //what;
//   // msg.regen_on = //what;
//   // msg.shutdown_esd_drain_open = //what;
//   // msg.shutdown_bms_open = //hwat;
//   // msg.shutdown_imd_open = //what;
//   // msg.shutdown_bspd_open = //what;
//   // msg.shutdown_vcu_open = //what;
//   // msg.shutdown_precharge_open = //what;
//   // msg.shutdown_master_reset_open = //what;
//   // msg.shutdown_driver_reset_open = //what;
//   // msg.heartbeat_front_can_node_dead = //what;
//   // msg.heartbeat_rear_can_node_dead = //what;
//   // msg.heartbeat_bms_dead = //what;
//   // msg.heartbeat_dash_dead = //what;
//   // msg.heartbeat_mc_dead = //what;
//   // msg.heartbeat_current_sensor_dead = //what;
//   // msg.tsms_off = //what;
//   // msg.reset_latch_open = //what;
//   // msg.precharge_running = //what;
//   // msg.master_reset_not_initialized = //what;
//   // msg.driver_reset_not_initialized = //what;
//   // msg.lv_battery_voltage = //what;
//   // msg.limp_state = //what;
//
//   can0_VcuToDash_Write(&msg);
// }

void sendTorqueCmdMsg(int16_t torque) {
  can0_MCCommand_T msg;

  msg.torque                        = torque;
  msg.speed                         = 0;
  msg.direction_is_counterclockwise = 0;
  msg.inverter_enabled              = 1; // INVERTER ENABLED
  msg.discharge_enabled             = 0;
  msg.torque_limit                  = 0;

  can0_MCCommand_Write(&msg);
}

void sendMotorOffCmdMsg() {
  can0_MCCommand_T msg;

  msg.torque                        = 0;
  msg.speed                         = 0;
  msg.direction_is_counterclockwise = 0;
  msg.inverter_enabled              = 0; // INVERTER DISABLED
  msg.discharge_enabled             = 0;
  msg.torque_limit                  = 0;

  can0_MCCommand_Write(&msg);
}
