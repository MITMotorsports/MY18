#include "can_handles.h"

void can_receive(Frame *input) {
  Can_RawRead(&input);
  can0_T msgForm = identify_can0(&input);

  switch (msgForm) {
  case can0_ChargerStatus1:
    handle_ChargerStatus1(&input);
    break;

  default:
    break;
  }
}

void handle_ChargerStatus1(Frame *input) {
  can0_ChargerStatus1_T unpacked_msg;
  unpack_can0_ChargerStatus1(input, &unpacked_msg);
  sensor_readings.current = unpacked_msg.DCCurrent;
  sensor_readings.voltage = unpacked_msg.DCVoltage;
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
