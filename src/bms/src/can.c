#include "can.h"

// CAN Initialization
void Board_CAN_Init() {
  init_can0_bms();
}

void Board_CAN_Receive(BMS_INPUT_T *bms_input) {
  Can_RawRead(&can_input);
  can0_T msgForm = identify_can0(&can_input);

  switch (msgForm) {
  case can0_BMSState:
    can_receive_bms_state(bms_input);
    break;

  case can0_BmsVcuSwitch:
    can_receive_vcu_switch(bms_input);
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

void can_receive_bms_state(BMS_INPUT_T *bms_input) {
  can0_BMSState_T msg;

  unpack_can0_BMSState(&can_input, &msg);
  bms_input->vcu_mode_request = msg.state;
}

void can_receive_vcu_switch(BMS_INPUT_T *bms_input) {
  can0_BmsVcuSwitch_T msg;

  unpack_can0_BmsVcuSwitch(&can_input, &msg);
  bms_input->vcu_switch = msg.always_true;
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
  uint32_t msTicks = bms_input->msTicks;

  can_transmit_contactor_weld(bms_input, msTicks);
  can_transmit_pack_status(bms_input, msTicks);
  can_transmit_bms_soc(bms_input, msTicks);

  // can_transmit_bms_errors(bms_input, msTicks);
}

void can_transmit_contactor_weld(BMS_INPUT_T *bms_input, uint32_t msTicks) {
  static uint32_t last_bms_contactor_weld_time = 0;

  if ((msTicks - last_bms_contactor_weld_time) > BMS_CONTACTOR_WELD_PERIOD) {
    can0_ContactorWeld_T msg;
    msg.one = bms_input->contactor_weld_one;
    msg.two = bms_input->contactor_weld_two;
    can0_ContactorWeld_Write(&msg);
    last_bms_contactor_weld_time = bms_input->msTicks;
  }
}

void can_transmit_pack_status(BMS_INPUT_T *bms_input, uint32_t msTicks) {
  static uint32_t last_bms_pack_status_time = 0;

  if ((msTicks - last_bms_pack_status_time) > BMS_PACK_STATUS_PERIOD) {
    can0_BmsPackStatus_T msg;

    /*

        Fill msg

     */

    can0_BmsPackStatus_Write(&msg);
    last_bms_pack_status_time = bms_input->msTicks;
  }
}

void can_transmit_bms_soc(BMS_INPUT_T *bms_input, uint32_t msTicks) {
  static uint32_t last_bms_soc_time = 0;

  if ((msTicks - last_bms_soc_time) > BMS_SOC_PERIOD) {
    can0_BMS_SOC_T msg;

    /*

        Fill msg

     */

    can0_BMS_SOC_Write(&msg);
    last_bms_soc_time = bms_input->msTicks;
  }
}

void can_transmit_bms_errors(BMS_INPUT_T *bms_input, uint32_t msTicks) {
  static uint32_t last_bms_errors_time = 0;

  if ((msTicks - last_bms_errors_time) > BMS_ERRORS_PERIOD) {
    /*

        Fill msg

     */

    ERROR_STATUS_T *start_index = Get_Errors();

    int i;

    for (i = 0; i < (ERROR_NUM_ERRORS + 2); i++) {
      can0_BMSErrors_T msg;

      if ((*(start_index + i)).error == true) { // TODO: Use arrow operator.
        msg.type = i + 1;
        can0_BMSErrors_Write(&msg);
      }
    }

    last_bms_errors_time = bms_input->msTicks;
  }
}
