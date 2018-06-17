#include "can_handles.h"

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan) {
  HAL_StatusTypeDef CAN_RX_STATUS = HAL_CAN_Receive_IT(hcan, CAN_FIFO0);

  if (CAN_RX_STATUS == HAL_OK) {
    handleCAN(hcan);
  }
  else {
    printf("[CAN RX] ERROR: HAL_StatusTypeDef is %d\r\n",    (int)CAN_RX_STATUS);
    printf("[CAN RX] ERROR: HAL_CAN_StateTypeDef is %d\r\n", hcan->State);
    printf("[CAN RX] ERROR: ErrorCode is %d\r\n",            hcan->ErrorCode);

    // handle_fatal_fault();
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

  case can0_CellVoltageRange:
    handleCellVoltagesMsg(&frame);
    break;

  case can0_CellTemperatureRange:
    handleCellTemperatures(&frame);
    break;

  case can0_CurrentSensor_Voltage1:
    handleCurrentSensorVoltageMsg(&frame);
    break;

  case can0_MCVoltage:
    handleMCVoltageMsg(&frame);
    break;

  case can0_ButtonRequest:
    handleButtonRequest(&frame);
    break;

  case can0_CurrentSensor_Power:
    handleCurrentSensor_Power(&frame);
    break;

  case can0_MCMotor_Position_Info:
    handleMCMotor_Position_Info(&frame);
    break;

  case can0_SBG_EKF_Velocity:
    handleSBG_EKF_Velocity(&frame);
    break;

  case can0_FrontCanNodeRightWheelSpeed:
    handleFrontCanNodeRightWheelSpeed(&frame);
    break;

  case can0_FrontCanNodeLeftWheelSpeed:
    handleFrontCanNodeLeftWheelSpeed(&frame);
    break;

  case can0_MCTorque_Timer_Info:
    handleMCTorque_Timer_Info(&frame);
    break;

  case can0_DashRequest:
    handleDashRequest(&frame);
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

void handleBMSHeartbeatMsg(Frame *frame) {
  can0_BMSHeartbeat_T msg;
  unpack_can0_BMSHeartbeat(frame, &msg);

  // msg.error_pec;
  // msg.error_cvst;
  // msg.error_owt;
  // msg.error_L_contactor_error;
  // msg.error_H_contactor_error;
  // msg.error_L_contactor_weld;
  // msg.error_H_contactor_weld;
  // msg.error_cell_under_voltage;
  // msg.error_cell_over_voltage;
  // msg.error_cell_under_temp;
  // msg.error_cell_over_temp;
  // msg.error_control_flow;
  bms_errors = msg;

  contactors.L_contactor_closed = msg.L_contactor_closed;
  contactors.H_contactor_closed = msg.H_contactor_closed;
  contactors.L_contactor_welded = msg.L_contactor_welded;
  contactors.H_contactor_welded = msg.H_contactor_welded;

  heartbeats.bms = HAL_GetTick();
}

void handleCurrentSensorVoltageMsg(Frame *msg) {
  can0_CurrentSensor_Voltage1_T unpacked_msg;

  unpack_can0_CurrentSensor_Voltage1(msg, &unpacked_msg);

  cs_readings.V_bus = unpacked_msg.result;

  heartbeats.current_sensor = HAL_GetTick();
}

void handleCellVoltagesMsg(Frame *msg) {
  can0_CellVoltageRange_T unpacked_msg;

  unpack_can0_CellVoltageRange(msg, &unpacked_msg);

  voltages.pack = unpacked_msg.sum / 100;  // mV -> dV

  cell_readings.cell_min_cV = unpacked_msg.min / 10;
}

void handleCellTemperatures(Frame *msg) {
  can0_CellTemperatureRange_T unpacked_msg;
  unpack_can0_CellTemperatureRange(msg, &unpacked_msg);

  if (cell_readings.temp_index != 0) {
    cell_readings.temp_index++;
  }
  cell_readings.temp_sum -= cell_readings.temp_log[cell_readings.temp_index];
  cell_readings.temp_log[cell_readings.temp_index] = unpacked_msg.max0 / 10;
  cell_readings.temp_sum += cell_readings.temp_log[cell_readings.temp_index];
}

void handleButtonRequest(Frame *msg) {
  can0_ButtonRequest_T unpacked_msg;

  unpack_can0_ButtonRequest(msg, &unpacked_msg);

  buttons.RTD          = unpacked_msg.RTD;
  buttons.DriverReset  = unpacked_msg.DriverReset;
}

void handleCurrentSensor_Power(Frame *msg) {
  can0_CurrentSensor_Power_T unpacked_msg;

  unpack_can0_CurrentSensor_Power(msg, &unpacked_msg);

  cs_readings.power = unpacked_msg.result;
}

void handleMCMotor_Position_Info(Frame *msg) {
  can0_MCMotor_Position_Info_T unpacked_msg;

  unpack_can0_MCMotor_Position_Info(msg, &unpacked_msg);

  mc_readings.speed = unpacked_msg.motor_speed;
}

void handleSBG_EKF_Velocity(Frame *msg) {
  can0_SBG_EKF_Velocity_T unpacked_msg;

  unpack_can0_SBG_EKF_Velocity(msg, &unpacked_msg);

  imu_velocity.north = unpacked_msg.north;
  imu_velocity.east = unpacked_msg.east;
}

void handleFrontCanNodeLeftWheelSpeed(Frame *msg) {
  can0_FrontCanNodeLeftWheelSpeed_T unpacked_msg;

  unpack_can0_FrontCanNodeLeftWheelSpeed(msg, &unpacked_msg);

  wheel_speeds.front_left_32b_wheel_speed = unpacked_msg.left_32b;
  wheel_speeds.front_left_16b_wheel_speed = unpacked_msg.left_16b;
}

void handleFrontCanNodeRightWheelSpeed(Frame *msg) {
  can0_FrontCanNodeRightWheelSpeed_T unpacked_msg;

  unpack_can0_FrontCanNodeRightWheelSpeed(msg, &unpacked_msg);

  wheel_speeds.front_right_32b_wheel_speed = unpacked_msg.right_32b;
  wheel_speeds.front_right_16b_wheel_speed = unpacked_msg.right_16b;
}

void handleMCTorque_Timer_Info(Frame *msg) {
  can0_MCTorque_Timer_Info_T unpacked_msg;

  unpack_can0_MCTorque_Timer_Info(msg, &unpacked_msg);

  mc_readings.torque_feedback = unpacked_msg.torque_feedback;
}

void handleDashRequest(Frame *msg) {
  can0_DashRequest_T unpacked_msg;

  unpack_can0_DashRequest(msg, &unpacked_msg);

  control_settings.using_regen = unpacked_msg.using_regen;
  control_settings.using_temp_limiting = unpacked_msg.using_temp_limiting;
  control_settings.using_voltage_limiting = unpacked_msg.using_voltage_limiting;


  if (unpacked_msg.regen_bias != 255) {
    control_settings.regen_bias = unpacked_msg.regen_bias;
    control_settings.using_regen = unpacked_msg.using_regen;
  }

  if (unpacked_msg.limp_factor != 255) {
    control_settings.limp_factor = unpacked_msg.limp_factor;
  }


  if (unpacked_msg.temp_lim_min_gain != 255 && unpacked_msg.temp_lim_thresh_temp != 255) {
    control_settings.temp_lim_min_gain = unpacked_msg.temp_lim_min_gain;
    control_settings.temp_lim_thresh_temp = unpacked_msg.temp_lim_thresh_temp;
  }

  if (unpacked_msg.volt_lim_min_gain != 255 && unpacked_msg.volt_lim_min_voltage != 65535) {
    control_settings.volt_lim_min_gain = unpacked_msg.volt_lim_min_gain;
    control_settings.volt_lim_min_voltage = unpacked_msg.volt_lim_min_voltage;
  }

  control_settings.active_aero_enabled = unpacked_msg.active_aero_enabled;
}

void send_VCUHeartbeat() {
  LIMIT(can0_VCUHeartbeat);

  can0_VCUHeartbeat_T msg = {};

  msg.vcu_state   = get_vcu_state();
  msg.error_state = get_error_state();
  msg.estop_hit   = gates.sdn;

  can0_VCUHeartbeat_Write(&msg);
}

void send_VCUErrors() {
  LIMIT(can0_VCUErrors);

  can0_VCUErrors_T msg = {};

  msg.fatal_gate      = fatal_faults.gate;
  msg.fatal_precharge = fatal_faults.precharge;
  msg.fatal_contactor = fatal_faults.contactor;
  msg.fatal_conflict  = fatal_faults.conflict;

  msg.recoverable_gate      = recoverable_faults.gate;
  msg.recoverable_heartbeat = recoverable_faults.heartbeat;
  msg.recoverable_conflict  = recoverable_faults.conflict;
  msg.recoverable_contactor = recoverable_faults.contactor;

  msg.gate_sdn = gates.sdn_gate;
  msg.gate_bms = gates.bms_gate;
  msg.gate_imd = gates.imd_gate;
  msg.gate_bpd = gates.bpd_gate;

  can0_VCUErrors_Write(&msg);
}

void send_VCUControlsParams() {
  LIMIT(can0_VCUControlsParams);

  can0_VCUControlsParams_Write(&control_settings);
}

void send_RawTorque() {
  LIMIT(can0_RawTorque);

  can0_RawTorque_Write(&can_raw_torque);
}

void send_VCU() {
  send_VCUHeartbeat();
  send_VCUErrors();
  send_VCUControlsParams();
  send_RawTorque();
}

void sendTorqueCmdMsg(int16_t torque) {
  LIMIT(can0_MCCommand);

  can0_MCCommand_T msg;

  msg.torque                        = torque;
  msg.angular_vel                   = 0;
  msg.direction_is_counterclockwise = false;
  msg.inverter_enabled              = true;
  msg.discharge_enabled             = false;
  msg.speed_mode                    = false;
  msg.torque_limit                  = 0;

  can0_MCCommand_Write(&msg);
}

void sendSpeedCmdMsg(int16_t speed, int16_t torque_limit) {
  LIMIT(can0_MCCommand);

  can0_MCCommand_T msg;
  if (speed < 0) speed = 0;

  msg.torque                        = 0;
  msg.angular_vel                   = speed;
  msg.direction_is_counterclockwise = false;
  msg.inverter_enabled              = true;
  msg.discharge_enabled             = false;
  msg.speed_mode                    = true;
  msg.torque_limit                  = torque_limit;

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
