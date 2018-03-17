#include "canmgr.h"
#include "error_handler.h"

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
  bms_input->vcu_mode_request = msg.state;
}

void can_receive_current(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Current_T msg;

  unpack_can0_CurrentSensor_Current(&can_input, &msg);
  bms_input->pack_status->pack_current_mA = msg.pack_current >
                                            0 ? msg.pack_current : -msg.
                                            pack_current;
}

void can_receive_voltage(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Voltage_T msg;

  unpack_can0_CurrentSensor_Voltage(&can_input, &msg);
  bms_input->pack_status->pack_voltage_mV = msg.pack_voltage >
                                            0 ? msg.pack_voltage : -msg.
                                            pack_voltage;
}

void can_receive_energy(BMS_INPUT_T *bms_input) {
  can0_CurrentSensor_Energy_T msg;

  unpack_can0_CurrentSensor_Energy(&can_input, &msg);
  bms_input->pack_status->pack_energy_wH = msg.pack_energy;
}

Frame can_output;
void Board_CAN_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output) {
  can_transmit_bms_heartbeat(bms_input);
  can_transmit_cell_voltages(bms_input);
  can_transmit_cell_temperatures(bms_input);

  // TODO: agree upon resending only if updated or
  // repeating existing info on bus (bms_input0->msTicks)
}

void can_transmit_bms_heartbeat(BMS_INPUT_T *bms_input) {
  static uint32_t last_time = 0;

  if ((msTicks - last_time) > can0_BMSHeartbeat_period) {
    const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

    ERROR_STATUS_T *errors = Get_Errors();
    can0_BMSHeartbeat_T msg;

    msg.error_pec = errors[ERROR_LTC6804_PEC].error ? true : false;
    msg.error_cvst = errors[ERROR_LTC6804_CVST].error ? true : false;
    msg.error_owt = errors[ERROR_LTC6804_OWT].error ? true : false;
    msg.error_eeprom = errors[ERROR_EEPROM].error ? true : false;
    msg.error_cell_under_voltage = errors[ERROR_CELL_UNDER_VOLTAGE].error ? true : false;
    msg.error_cell_over_voltage = errors[ERROR_CELL_OVER_VOLTAGE].error ? true : false;
    msg.error_cell_under_temp = errors[ERROR_CELL_UNDER_TEMP].error ? true : false;
    msg.error_cell_over_temp = errors[ERROR_CELL_OVER_TEMP].error ? true : false;
    msg.error_over_current = errors[ERROR_OVER_CURRENT].error ? true : false;
    msg.error_can = errors[ERROR_CAN].error ? true : false;
    msg.error_conflicting_mode_requests = errors[ERROR_CONFLICTING_MODE_REQUESTS].error ? true : false;
    msg.error_vcu_dead = errors[ERROR_VCU_DEAD].error ? true : false;
    msg.error_control_flow = errors[ERROR_CONTROL_FLOW].error ? true : false;
    msg.error_blown_fuse = errors[ERROR_BLOWN_FUSE].error ? true : false;
    msg.error_contactor_welded = errors[ERROR_CONTACTOR_WELDED].error ? true : false;

    msg.soc = ps->state_of_charge;
    can0_BMSHeartbeat_Write(&msg);

    // ERROR_STATUS_T *start_index = Get_Errors();

    // for (int i = 0; i < (ERROR_NUM_ERRORS + 2); i++) {
    //   can0_BMSHeartbeat_T msg;

    //   if ((start_index + i)->error == true) {
    //     msg.type = i + 1;
    //     msg.soc = ps->state_of_charge;
    //     can0_BMSHeartbeat_Write(&msg);
    //   }
    // }

    last_time = msTicks;
  }
}

void can_transmit_cell_voltages(BMS_INPUT_T *bms_input) {
  static uint32_t last_time = 0;

  if ((msTicks - last_time) > can0_CellVoltages_period) {
    const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

    can0_CellVoltages_T msg;
    msg.min = ps->pack_cell_min_mV;
    msg.argmin = 1;
    msg.max = ps->pack_cell_max_mV;
    msg.argmax = 1;

    can0_CellVoltages_Write(&msg);

    last_time = msTicks;
  }
}

void can_transmit_cell_temperatures(BMS_INPUT_T *bms_input) {
  static uint32_t last_time = 0;

  if ((msTicks - last_time) > can0_CellTemperatures_period) {
    const BMS_PACK_STATUS_T *ps = bms_input->pack_status;

    can0_CellTemperatures_T msg;
    msg.min = ps->min_cell_temp_dC;
    msg.argmin = ps->min_cell_temp_position;
    msg.max = ps->max_cell_temp_dC;
    msg.argmax = ps->max_cell_temp_position;

    can0_CellTemperatures_Write(&msg);

    last_time = msTicks;
  }
}
