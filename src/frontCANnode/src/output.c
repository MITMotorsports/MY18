#include "output.h"
#include "serial.h"

#include "chip.h"
#include "can.h"

#include <CANlib.h>

const uint8_t can_brakethrottle_period = can0_FrontCanNodeBrakeThrottle_period; //20; //1000 / can0_FrontCanNodeBrakeThrottle_frequency;
const uint8_t can_wheel_speed_period = can0_FrontCanNodeWheelSpeed_period; //100; //1000 / can0_FrontCanNodeWheelSpeed_frequency;

static bool resettingPeripheral = false;

Can_ErrorID_T write_can_brakethrottle_msg(Input_T *input);
Can_ErrorID_T write_can_wheel_speed_msg(Input_T *input);
void handle_can_error(Can_ErrorID_T error);
bool period_reached(uint32_t start, uint32_t period, uint32_t msTicks);

void Output_initialize(Output_T *output) {
  output->can_brakethrottle_ms = 0;
  output->can_wheel_speed_ms = 0;
  output->send_brakethrottle_msg = false;
  output->send_wheel_speed_msg = false;
}

void Output_process_output(Input_T *input, Output_T *output) {
  if(period_reached(output->can_brakethrottle_ms, can_brakethrottle_period, input->msTicks)) {
    output->can_brakethrottle_ms = input->msTicks;
    output->send_brakethrottle_msg = true;
  }

  if(period_reached(output->can_wheel_speed_ms, can_wheel_speed_period, input->msTicks)) {
    output->can_wheel_speed_ms = input->msTicks;
    output->send_wheel_speed_msg = true;
  }

  if(output->send_brakethrottle_msg) {
    output->send_brakethrottle_msg = false;
    Can_ErrorID_T error = write_can_brakethrottle_msg(input);
    handle_can_error(error);
  }

  if(output->send_wheel_speed_msg) {
    output->send_wheel_speed_msg = false;
    Can_ErrorID_T error = write_can_wheel_speed_msg(input);
    handle_can_error(error);
  }
}

Can_ErrorID_T write_can_brakethrottle_msg(Input_T *input) {
  can0_FrontCanNodeBrakeThrottle_T msg;

  msg.brake_1 = input->adc->brake_1;
  msg.brake_2 = input->adc->brake_2;
  msg.accel_1 = input->adc->accel_1;
  msg.accel_2 = input->adc->accel_2;

  return can0_FrontCanNodeBrakeThrottle_Write(&msg);
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

bool period_reached(uint32_t start, uint32_t period, uint32_t msTicks) {
  const uint32_t next_time = start + period;
  return next_time < msTicks;
}
