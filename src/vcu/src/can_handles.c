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

  cs_readings.V_bus = unpacked_msg.voltage;

  heartbeats.current_sensor = HAL_GetTick();
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

void sendHeartbeatMsg() {
  LIMIT(can0_VCUHeartbeat);

  can0_VCUHeartbeat_T msg;

  msg.vcu_state = get_vcu_state();
  msg.error_state = get_error_state();

  can0_VCUHeartbeat_Write(&msg);
}


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

void send_mc_fault_clear() {
  can0_MCParameterRequest_T msg;

  // RMS CAN protocol page 34

  msg.address = 20;
  msg.write   = true;
  msg.data    = 0;

  can0_MCParameterRequest_Write(&msg);
}
