#include "chip.h"
#include "CANlib.h"
#include "serial.h"
#include "adc17.h"

#define CAN_BAUDRATE 500000
#define CAN_PERIOD_MS 10

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

const int16_t k_p = 114957 / 50000;
int16_t setpoint = 1500;

Frame latest_frame;
can0_MC_MotorPositionInformation_T latest_msg;

void SysTick_Handler(void) {
  msTicks++;
}

int16_t get_torque(int16_t motor_speed, int16_t setpoint) {
  int16_t delta = motor_speed - setpoint;
  // Original value: 114957 / 10000
  int16_t torque = delta * k_p;
  return torque;
}

uint16_t get_accel(void) {
  uint16_t accel_1_raw = ADC_Read(ACCEL_1_CHANNEL);
  uint16_t accel_2_raw = ADC_Read(ACCEL_1_CHANNEL);
  uint16_t accel_1 = transform1(accel_1_raw);
  uint16_t accel_2 = transform2(accel_2_raw);
  return accel_1 > accel_2 ? accel_2 : accel_1;
}

Can_ErrorID_T enable_mc(void) {
  can0_MC_Command_T msg;
  msg.torque = 0;
  msg.direction_is_counterclockwise = 0;
  msg.inverter_enabled = 1;
  msg.discharge_enabled = 0;
  msg.torque_limit = 0;
  return can0_MC_Command_Write(&msg);
}


Can_ErrorID_T disable_mc(void) {
  can0_MC_Command_T msg;
  msg.torque = 0;
  msg.direction_is_counterclockwise = 0;
  msg.inverter_enabled = 0;
  msg.discharge_enabled = 0;
  msg.torque_limit = 0;
  return can0_MC_Command_Write(&msg);
}


Can_ErrorID_T send_torque(uint16_t torque, bool direction) {
  can0_MC_Command_T msg;
  msg.torque = torque;
  msg.direction_is_counterclockwise = direction;
  msg.inverter_enabled = 1;
  msg.discharge_enabled = 0;
  msg.torque_limit = 0;
  return can0_MC_Command_Write(&msg);
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
        Serial_Print("Can_Error_TX_BUFFER_FULL\n");
        break;
      case Can_Error_RX_BUFFER_FULL:
        Serial_Print("Can_Error_RX_BUFFER_FULL\n");
        break;
    }
  }
}


int main(void) {
  SystemCoreClockUpdate();

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  Can_Init(CAN_BAUDRATE);
  Serial_Init(57600);
  ADC_Init();

  handle_can_error(enable_mc());

  uint32_t next_time = msTicks;

  while(1) {
    // Always read CAN message
    Can_RawRead(&latest_frame);
    can0_T msgForm = identify_can0(&latest_frame);
    switch(msgForm) {
      case can0_MC_MotorPositionInformation:
        unpack_can0_MC_MotorPositionInformation(&latest_frame, &latest_msg);
        break;
      default: // Gets rid of annoying compilation error
        break;
    }

    uint16_t accel = get_accel();
    int16_t torque;

    // Calculate torque
    if (accel > 50) { // Cutoff based off of implausibility
      // msg.torque = get_torque(latest_msg.motor_speed, setpoint);
      torque = 0;
    } else {
      handle_can_error(disable_mc());
      torque = 0;
    }

    bool period_reached = msTicks >= next_time;

    // Write message (only at the specificed time interval)
    if (period_reached) {
      next_time = msTicks + CAN_PERIOD_MS;
      handle_can_error(send_torque(torque, 1));
    }
  }
}
