#include "canmgr.h"

// CAN Initialization
void Board_CAN_Init() {
  init_can0_bms();

  // Only accept current sensor energy for SoC calculation
  // HACK: Infrequent messages such as CellTemperatures get cleared out of the
  //       TX buffer due to Can_Error_RX_BUFFER_FULL occurring before transmission
  //       thus reinitializing CAN and emptying the TX buffer.
  // Can_SetFilter(2047, can0_CurrentSensor_Energy_can_id);
  // Can_SetFilter(0, 1);  // Disable ALL incoming messages
}

void Board_CAN_Receive(BMS_INPUT_T *bms_input) {
  Can_RawRead(&can_input);
  can0_T msgForm = identify_can0(&can_input);

  switch (msgForm) {
  case can0_BMSRequest:
    can_receive_bms_request(bms_input);
    break;

  case can0_CurrentSensor_Current:
    can_receive_current(bms_input);
    break;

  case can0_CurrentSensor_Voltage1:
    can_receive_voltage(bms_input);
    break;

  case can0_CurrentSensor_Energy:
    can_receive_energy(bms_input);
    break;

  case CAN_UNKNOWN_MSG:
    break;

  default:
    break;
  }
}

void can_receive_bms_request(BMS_INPUT_T *bms_input) {
  can0_BMSRequest_T msg;

  unpack_can0_BMSRequest(&can_input, &msg);
  if (msg.force_error) {
    Error_Present(ERROR_FORCE_OVER_CAN);
  }
  else {
    Error_Clear(ERROR_FORCE_OVER_CAN);
  }
}

void can_receive_current(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Current_T msg;

  unpack_can0_CurrentSensor_Current(&can_input, &msg);
  bms_input->pack_status->pack_current_mA = msg.result;
}

void can_receive_voltage(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Voltage1_T msg;

  unpack_can0_CurrentSensor_Voltage1(&can_input, &msg);
  bms_input->pack_status->pack_voltage_mV = msg.result;
}

void can_receive_energy(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Energy_T msg;

  unpack_can0_CurrentSensor_Energy(&can_input, &msg);
  bms_input->pack_status->pack_energy_wH = msg.result;
}

Frame can_output;
void Board_CAN_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output) {
  can_transmit_bms_heartbeat(bms_input);
  can_transmit_cell_voltages(bms_input);
  can_transmit_cell_temperatures(bms_input);
}

void can_transmit_bms_heartbeat(BMS_INPUT_T *bms_input) {

  LIMIT(can0_BMSHeartbeat_period);

  can0_BMSHeartbeat_T msg = {};

  msg.error_pec = Check_Error(ERROR_LTC_PEC, false);

  msg.error_cvst = Check_Error(ERROR_LTC_CVST, false);
  msg.error_owt = Check_Error(ERROR_LTC_OWT, false);
  msg.error_L_contactor_error = Check_Error(ERROR_L_CONTACTOR_ERROR, false);
  msg.error_H_contactor_error = Check_Error(ERROR_H_CONTACTOR_ERROR, false);
  msg.error_L_contactor_weld = Check_Error(ERROR_L_CONTACTOR_WELD, false);
  msg.error_H_contactor_weld = Check_Error(ERROR_H_CONTACTOR_WELD, false);
  msg.error_cell_under_voltage = Check_Error(ERROR_CELL_UNDER_VOLTAGE, false);
  msg.error_cell_over_voltage = Check_Error(ERROR_CELL_OVER_VOLTAGE, false);
  msg.error_cell_over_temp = Check_Error(ERROR_CELL_OVER_TEMP, false);
  msg.error_control_flow = Check_Error(ERROR_CONTROL_FLOW, false);
  msg.error_force_over_can = Check_Error(ERROR_FORCE_OVER_CAN, false);
  msg.L_contactor_closed = bms_input->L_contactor_closed;
  msg.H_contactor_closed = bms_input->H_contactor_closed;
  msg.L_contactor_welded = bms_input->L_contactor_welded;
  msg.H_contactor_welded = bms_input->H_contactor_welded;

  msg.soc = 255;

  handle_can_error(can0_BMSHeartbeat_Write(&msg));
}



void can_transmit_cell_voltages(BMS_INPUT_T *bms_input) {
  LIMIT(can0_CellVoltageRange_period);

  const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

  // TODO: Get info about argmin/argmax.
  can0_CellVoltageRange_T msg = {};

  msg.min = ps->pack_cell_min_mV;
  msg.max = ps->pack_cell_max_mV;
  msg.sum = ps->pack_voltage_sum_mV;

  handle_can_error(can0_CellVoltageRange_Write(&msg));
}

void can_transmit_cell_temperature_range(BMS_INPUT_T *bms_input) {
  LIMIT(can0_CellTemperatureRange_period);

  const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

  can0_CellTemperatureRange_T msg;

  #define maxT (ps->max_cell_temp_dC)
  #define minT (ps->min_cell_temp_dC)

  msg.min     = minT[0].val;
  msg.argmin  = minT[0].idx;

  msg.max0    = maxT[0].val;
  msg.argmax0 = maxT[0].idx;

  msg.max1    = maxT[1].val;
  msg.argmax1 = maxT[1].idx;

  msg.max2    = maxT[2].val;

  #undef maxT
  #undef minT

  handle_can_error(can0_CellTemperatureRange_Write(&msg));
}

void can_transmit_cell_temperature_variance(BMS_INPUT_T *bms_input) {
  LIMIT(can0_CellTemperatureVariance_period);

  const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

  can0_CellTemperatureVariance_T msg;

  msg.mean = ps->avg_cell_temp_dC;
  msg.variance = ps->variance_cell_temp;

  handle_can_error(can0_CellTemperatureVariance_Write(&msg));
}

void can_transmit_cell_temperatures(BMS_INPUT_T *bms_input) {
  can_transmit_cell_temperature_range(bms_input);
  can_transmit_cell_temperature_variance(bms_input);
}

void handle_can_error(Can_ErrorID_T error) {
  if ((error != Can_Error_NONE) && (error != Can_Error_NO_RX)) {
    switch (error) {
    case Can_Error_NONE:
      Board_Print("Can_Error_NONE\n");
      break;

    case Can_Error_NO_RX:
      Board_Print("Can_Error_NO_RX\n");
      break;

    case Can_Error_EPASS:
      Board_Print("Can_Error_EPASS\n");
      break;

    case Can_Error_WARN:
      Board_Print("Can_Error_WARN\n");
      break;

    case Can_Error_BOFF:
      Board_Print("Can_Error_BOFF\n");
      break;

    case Can_Error_STUF:
      Board_Print("Can_Error_STUF\n");
      break;

    case Can_Error_FORM:
      Board_Print("Can_Error_FORM\n");
      break;

    case Can_Error_ACK:
      Board_Print("Can_Error_ACK\n");
      break;

    case Can_Error_BIT1:
      Board_Print("Can_Error_BIT1\n");
      break;

    case Can_Error_BIT0:
      Board_Print("Can_Error_BIT0\n");
      break;

    case Can_Error_CRC:
      Board_Print("Can_Error_CRC\n");
      break;

    case Can_Error_UNUSED:
      Board_Print("Can_Error_UNUSED\n");
      break;

    case Can_Error_UNRECOGNIZED_MSGOBJ:
      Board_Print("Can_Error_UNRECOGNIZED_MSGOBJ\n");
      break;

    case Can_Error_UNRECOGNIZED_ERROR:
      Board_Print("Can_Error_UNRECOGNIZED_ERROR\n");
      break;

    case Can_Error_TX_BUFFER_FULL:
      // Board_Print("Can_Error_TX_BUFFER_FULL\n");
      CAN_Flush_Tx();
      CAN_Clear_Error();
      CAN_ResetPeripheral();
      init_can0_bms();
      break;

    case Can_Error_RX_BUFFER_FULL:
      // Board_Print("Can_Error_RX_BUFFER_FULL\n");
      CAN_Flush_Tx();
      CAN_Clear_Error();
      CAN_ResetPeripheral();
      init_can0_bms();
      break;
    }
  }
}
