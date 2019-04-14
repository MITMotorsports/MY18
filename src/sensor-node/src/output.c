#include "output.h"
#include "serial.h"

#include "chip.h"
#include "can.h"

#include <CANlib.h>

extern Input_T input;
extern volatile uint32_t msTicks;

void write_can_brakethrottle_msg(void);
void write_can_left_wheel_speed_msg(void);
void write_can_right_wheel_speed_msg(void);
void handle_can_error(Can_ErrorID_T error);
bool period_reached(uint32_t start, uint32_t period, uint32_t msTicks);

void Output_process_output() {
  write_can_steering_msg();
}

void write_can_steering_msg() {
  LIMIT(can0_FrontCanNodeSteering_period);

  can0_FrontCanNodeSteering_T msg;

  msg.value = input.adc->steering_pot;

  handle_can_error(can0_FrontCanNodeSteering_Write(&msg));
}

void handle_can_error(Can_ErrorID_T error) {
  if ((error != Can_Error_NONE) && (error != Can_Error_NO_RX)) {
    switch (error) {
    case Can_Error_NONE:
      Serial_Print("Can_Error_NONE\n");
      break;

    case Can_Error_NO_RX:
      Serial_Print("Can_Error_NO_RX\n");
      break;

    case Can_Error_EPASS:
      Serial_Print("Can_Error_EPASS\n");
      break;

    case Can_Error_WARN:
      Serial_Print("Can_Error_WARN\n");
      break;

    case Can_Error_BOFF:
      Serial_Print("Can_Error_BOFF\n");
      break;

    case Can_Error_STUF:
      Serial_Print("Can_Error_STUF\n");
      break;

    case Can_Error_FORM:
      Serial_Print("Can_Error_FORM\n");
      break;

    case Can_Error_ACK:
      Serial_Print("Can_Error_ACK\n");
      break;

    case Can_Error_BIT1:
      Serial_Print("Can_Error_BIT1\n");
      break;

    case Can_Error_BIT0:
      Serial_Print("Can_Error_BIT0\n");
      break;

    case Can_Error_CRC:
      Serial_Print("Can_Error_CRC\n");
      break;

    case Can_Error_UNUSED:
      Serial_Print("Can_Error_UNUSED\n");
      break;

    case Can_Error_UNRECOGNIZED_MSGOBJ:
      Serial_Print("Can_Error_UNRECOGNIZED_MSGOBJ\n");
      break;

    case Can_Error_UNRECOGNIZED_ERROR:
      Serial_Print("Can_Error_UNRECOGNIZED_ERROR\n");
      break;

    case Can_Error_TX_BUFFER_FULL:
      Serial_Print("Can_Error_TX_BUFFER_FULL\n");
      CAN_Init(500000);
      break;

    case Can_Error_RX_BUFFER_FULL:
      Serial_Print("Can_Error_RX_BUFFER_FULL\n");
      CAN_Init(500000);
      break;
    }
  }
}
