#include "can_handles.h"


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan) {
  HAL_StatusTypeDef CAN_RX_STATUS = HAL_CAN_Receive_IT(hcan, CAN_FIFO0);

  if (CAN_RX_STATUS == HAL_OK) {
    handleCAN(hcan);
  }
  else {
    printf("[CAN RX] ERROR: HAL_StatusTypeDef is %d\r\n", (int)CAN_RX_STATUS);
    printf("[CAN RX] ERROR: HAL_CAN_StateTypeDef is %d\r\n", hcan->State);
    printf("[CAN RX] ERROR: ErrorCode is %d\r\n", hcan->ErrorCode);

    handle_fatal_fault();
  }
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef *hcan) {
  // printf("TX CB CALLED\r\n");
}

// void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
//   printf("[CAN ERR] %d\r\n", (int)hcan->State);
// }

void handleCAN(CAN_HandleTypeDef *hcan) {
  Frame frame;

  // TODO: Maybe use hcan to get last RxMsg here.

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

  case can0_ButtonRequest:
    handleButtonRequest(&frame);
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

  mc_readings.V_bus    = unpacked_msg.bus;
  mc_readings.V_out    = unpacked_msg.out;
  mc_readings.V_VAB_Vd = unpacked_msg.VAB_Vd;
  mc_readings.V_VBC_Vq = unpacked_msg.VBC_Vq;

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

  cs_readings.V_bus = unpacked_msg.dc_bus_voltage;
}

void handleCellVoltagesMsg(Frame *msg) {
  can0_CellVoltages_T unpacked_msg;

  unpack_can0_CellVoltages(msg, &unpacked_msg);

  // So we take the cell voltage of the minimum cell and use that
  // to estimate the lower bound on the back voltage
  // (12 per cell, 6 cells, millivolts to decivolts)
  voltages.pack = unpacked_msg.max * 12 * 6 / 100;
}

void handleButtonRequest(Frame *msg) {
  // can0_ButtonRequest_T unpacked_msg;
  //
  // unpack_can0_ButtonRequest(msg, &unpacked_msg);
  //
  // buttons.RTD          = unpacked_msg.RTD;
  // buttons.DriverReset  = unpacked_msg.DriverReset;
  // buttons.ScrollSelect = unpacked_msg.ScrollSelect;

  // TODO/HACK: Fix CANlib and replace correct unpacker.
  buttons.RTD         = (msg->data[0] & 2) != 0;
  buttons.DriverReset = (msg->data[0] & 4) != 0;
}

// void sendDashMsg() {
//   can0_VcuToDash_T msg;
//
//   // msg.rtd_light_on = //is this right? (carMode == VCU_STATE_RTD
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
//   // msg.heartbeat_get_sensor_dead = //what;
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
  LIMIT(can0_MCCommand);

  can0_MCCommand_T msg;

  msg.torque                        = torque;
  msg.angular_vel                   = 0;
  msg.direction_is_counterclockwise = 1;
  msg.inverter_enabled              = true;
  msg.discharge_enabled             = false;
  msg.speed_mode                    = false;
  msg.torque_limit                  = 0;

  can0_MCCommand_Write(&msg);
}

void sendMotorOffCmdMsg() {
  LIMIT(can0_MCCommand);

  can0_MCCommand_T msg;

  msg.torque                        = 0;
  msg.angular_vel                   = 0;
  msg.direction_is_counterclockwise = 0;
  msg.inverter_enabled              = false;
  msg.discharge_enabled             = false;
  msg.speed_mode                    = false;
  msg.torque_limit                  = 0;

  can0_MCCommand_Write(&msg);
}
