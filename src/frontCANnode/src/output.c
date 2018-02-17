#include "output.h"
#include "serial.h"

#include "chip.h"
#include "can.h"

#include <CANlib.h>

static bool resettingPeripheral = false;

Can_ErrorID_T write_can_output_msg(Input_T *input, State_T *state);
Can_ErrorID_T write_can_wheel_speed_msg(Input_T *input);
void handle_can_error(Can_ErrorID_T error);

void Output_initialize(Can_Output_State_T *output) {
  output->can_output_ms = 0;
  output->can_wheel_speed_ms = 0;
  output->send_output_msg = false;
  output->send_wheel_speed_msg = false;
}

void Output_process_output(Input_T *input, State_T *state) {
  if(state->can_output->send_output_msg) {
    state->can_output->send_output_msg = false;
    Can_ErrorID_T error = write_can_output_msg(input, state);
    handle_can_error(error);
  }

  if(state->can_output->send_wheel_speed_msg) {
    state->can_output->send_wheel_speed_msg = false;
    Can_ErrorID_T error = write_can_wheel_speed_msg(input);
    handle_can_error(error);
  }
}

Can_ErrorID_T write_can_output_msg(Input_T *input, State_T *state) {
  can0_FrontCanNodeOutput_T msg;

  msg.has_brake_throttle_conflict = state->rules->has_brake_throttle_conflict;
  msg.is_throttle_implausible = state->rules->implausibility_reported;
  msg.requested_torque = state->torque->requested_torque;

  return can0_FrontCanNodeOutput_Write(&msg);
}

Can_ErrorID_T write_can_wheel_speed_msg(Input_T *input) {

  can0_FrontCanNodeWheelSpeed_T msg;

  msg.front_right_wheel_speed = input->speed->front_right_wheel_speed;
  msg.front_left_wheel_speed = input->speed->front_left_wheel_speed;

  return can0_FrontCanNodeWheelSpeed_Write(&msg);
}

void handle_can_error(Can_ErrorID_T error) {
  if (error != Can_Error_NONE && error != Can_Error_NO_RX) { // Unimorportant errors
    switch(error) {
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
        //Serial_Print("Can_Error_TX_BUFFER_FULL\n");
        break;
      case Can_Error_RX_BUFFER_FULL:
        Serial_Print("Can_Error_RX_BUFFER_FULL\n");
        break;
    }
    if (!resettingPeripheral) {
      resettingPeripheral = true;
      // TODO add this to CAN library
      CAN_ResetPeripheral();
      Can_Init(500000);
    }
  } else {
    resettingPeripheral = false;
  }
}
