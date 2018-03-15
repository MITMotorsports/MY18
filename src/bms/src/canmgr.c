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
    ERROR_STATUS_T *start_index = Get_Errors();

    for (int i = 0; i < (ERROR_NUM_ERRORS + 2); i++) {
      can0_BMSHeartbeat_T msg;

      if ((start_index + i)->error == true) { // TODO: Use arrow operator.
        msg.type = i + 1;
        msg.soc = ps->state_of_charge;
        can0_BMSHeartbeat_Write(&msg);
      }
    }

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
