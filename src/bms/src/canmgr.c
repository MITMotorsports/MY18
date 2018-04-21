#include "canmgr.h"

// CAN Initialization
void Board_CAN_Init() {
  init_can0_bms();
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

  case can0_CurrentSensor_Voltage:
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
}

void can_receive_current(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Current_T msg;

  unpack_can0_CurrentSensor_Current(&can_input, &msg);
  bms_input->pack_status->pack_current_mA = msg.current;
}

void can_receive_voltage(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Voltage_T msg;

  unpack_can0_CurrentSensor_Voltage(&can_input, &msg);
  bms_input->pack_status->pack_voltage_mV = msg.voltage;
}

void can_receive_energy(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Energy_T msg;

  unpack_can0_CurrentSensor_Energy(&can_input, &msg);
  bms_input->pack_status->pack_energy_wH = msg.energy;
}

Frame can_output;
void Board_CAN_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output) {
  Can_ErrorID_T err[3];

  err[0] = can_transmit_bms_heartbeat(bms_input);
  err[1] = can_transmit_cell_voltages(bms_input);
  err[2] = can_transmit_cell_temperatures(bms_input);

  for (size_t i = 0; i < 3; ++i) {
    switch (err[i]) {
      case Can_Error_NONE:
      case Can_Error_UNUSED:
        break;
      default:
        Board_Print_BLOCKING("CAN SEND ERROR in msg ");
        Board_PrintNum(i, 10);
        Board_Print_BLOCKING("it was ");
        Board_PrintNum((int) err[i], 10);
        Board_Println_BLOCKING("IMA FLUSH THE BUFFER THX");
        CAN_Flush_Tx();
        CAN_Clear_Error();
        CAN_ResetPeripheral();
        init_can0_bms();
    }
  }



  // TODO: agree upon resending only if updated or
  // repeating existing info on bus (bms_input0->msTicks)
}

Can_ErrorID_T can_transmit_bms_heartbeat(BMS_INPUT_T *bms_input) {
  static uint32_t last_time = 0;

  Can_ErrorID_T err = Can_Error_UNUSED;

  if ((msTicks - last_time) > can0_BMSHeartbeat_period) {

    can0_BMSHeartbeat_T msg;

    msg.error_pec = Check_Error(ERROR_LTC_PEC, false);
    msg.error_cvst = Check_Error(ERROR_LTC_CVST, false);
    msg.error_owt = Check_Error(ERROR_LTC_OWT, false);
    msg.error_L_contactor_error = Check_Error(ERROR_L_CONTACTOR_ERROR, false);
    msg.error_H_contactor_error = Check_Error(ERROR_H_CONTACTOR_ERROR, false);
    msg.error_L_contactor_weld = Check_Error(ERROR_L_CONTACTOR_WELD, false);
    msg.error_H_contactor_weld = Check_Error(ERROR_H_CONTACTOR_WELD, false);
    msg.error_cell_under_voltage = Check_Error(ERROR_CELL_UNDER_VOLTAGE, false);
    msg.error_cell_over_voltage = Check_Error(ERROR_CELL_OVER_VOLTAGE, false);
    msg.error_cell_under_temp = Check_Error(ERROR_CELL_UNDER_TEMP, false);
    msg.error_cell_over_temp = Check_Error(ERROR_CELL_OVER_TEMP, false);
    msg.error_control_flow = Check_Error(ERROR_CONTROL_FLOW, false);

    msg.L_contactor_closed = bms_input->L_contactor_closed;
    msg.H_contactor_closed = bms_input->H_contactor_closed;
    msg.L_contactor_welded = bms_input->L_contactor_welded;
    msg.H_contactor_welded = bms_input->H_contactor_welded;

    // msg.soc = 7;
    err = can0_BMSHeartbeat_Write(&msg);

    last_time = msTicks;
  }

  return err;
}

Can_ErrorID_T can_transmit_cell_voltages(BMS_INPUT_T *bms_input) {
  static uint32_t last_time = 0;
  Can_ErrorID_T err = Can_Error_UNUSED;

  if ((msTicks - last_time) > can0_CellVoltages_period) {
    const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

    // TODO: Get info about argmin/argmax.
    can0_CellVoltages_T msg;

    msg.min = ps->pack_cell_min_mV;
    msg.max = ps->pack_cell_max_mV;

    //can message size too small for 3 32 bit
    // msg.sum = ps->pack_voltage_sum_mV;
    err = can0_CellVoltages_Write(&msg);

    last_time = msTicks;
  }

  return err;
}

Can_ErrorID_T can_transmit_cell_temperatures(BMS_INPUT_T *bms_input) {
  static uint32_t last_time = 0;

  Can_ErrorID_T err = Can_Error_UNUSED;

  if ((msTicks - last_time) > can0_CellTemperatures_period) {
    const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

    can0_CellTemperatures_T msg;
    msg.min = ps->min_cell_temp_dC;
    msg.argmin = ps->min_cell_temp_position;
    msg.max = ps->max_cell_temp_dC;
    msg.argmax = ps->max_cell_temp_position;

    err = can0_CellTemperatures_Write(&msg);

    last_time = msTicks;
  }

  return err;
}
