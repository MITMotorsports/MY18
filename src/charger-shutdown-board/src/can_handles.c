#include "can_handles.h"

void can_receive(Frame *input) {
  Can_RawRead(&input);
  can0_T msgForm = identify_can0(&input);

  switch (msgForm) {
  case can0_ChargerStatus1:
    handle_ChargerStatus1(&input);
    break;

  case can0_CurrentSensor_Current:
    handle_CurrentSensor_Current(&input);
    break;

  case can0_CurrentSensor_Voltage1:
    handle_CurrentSensor_Voltage1(&input);
    break;

  case can0_BMSHeartbeat:
    handle_BMSHeartbeat(&input);
    break;

  case can0_CellVoltages:
    handle_CellVoltages(&input);
    break;

  default:
    break;
  }
}

void handle_CurrentSensor_Current(Frame *input) {
  if (USING_CURRENT_SENSOR) {
    can0_CurrentSensor_Current_T unpacked_msg;
    unpack_can0_CurrentSensor_Current(input, &unpacked_msg);
    sensor_readings.current = unpacked_msg.current;
  }
}

void handle_CurrentSensor_Voltage1(Frame *input) {
  if (USING_CURRENT_SENSOR) {
    can0_CurrentSensor_Voltage1_T unpacked_msg;
    unpack_can0_CurrentSensor_Voltage1(input, &unpacked_msg);
    sensor_readings.voltage = unpacked_msg.result;
  }
}

void handle_ChargerStatus1(Frame *input) {
  if (!USING_CURRENT_SENSOR) {
    can0_ChargerStatus1_T unpacked_msg;
    unpack_can0_ChargerStatus1(input, &unpacked_msg);
    sensor_readings.current = unpacked_msg.DCCurrent;
    sensor_readings.voltage = unpacked_msg.DCVoltage;
  }
}

void handle_BMSHeartbeat(Frame *input) {
  can0_BMSHeartbeat_T unpacked_msg;

  unpack_can0_BMSHeartbeat(input, &unpacked_msg);
  bms_state.L_contactor_closed = unpacked_msg.L_contactor_closed;
}

void handle_CellVoltages(Frame *input) {
  can0_CellVoltages_T unpacked_msg;

  unpack_can0_CellVoltages(input, &unpacked_msg);
  bms_state.min_cell_voltage = unpacked_msg.min;
  bms_state.max_cell_voltage = unpacked_msg.max;
}

Can_ErrorID_T send_ChargerCommand(bool enabled) {
  LIMIT(can0_ChargerCommand_period);

  can0_ChargerCommand_T msg;

  msg.enable         = enabled;
  msg.powerReference = 1000; // 10 * percentage
  msg.VoltageLimit   = 3000; // 10 * V
  msg.CurrentLimit   = 100;  // 10 * A

  handle_can_error(can0_ChargerCommand_Write(&msg));
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
      Board_Print("Can_Error_TX_BUFFER_FULL\n");
      CAN_Init(500000);
      break;

    case Can_Error_RX_BUFFER_FULL:
      Board_Print("Can_Error_RX_BUFFER_FULL\n");
      CAN_Init(500000);
      break;
    }
  }
}
