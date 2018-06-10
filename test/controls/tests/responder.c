#include "chip.h"
#include "Can_Library.h"
#include <string.h>
#include "serial.h"

#define SERIAL_BAUDRATE 57600
#define CAN_BAUDRATE 500000

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

void SysTick_Handler(void) {
  msTicks++;
}

uint32_t math(uint32_t num) {
  int i;
  for(i = 0; i < 100; i++) {
    num += 3;
  }
  return num;
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);
  Can_Init(CAN_BAUDRATE);

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  while(1) {
    Can_FrontCanNodeWheelSpeed_T tx_msg;
    Can_FrontCanNodeWheelSpeed_T rx_msg;

    Can_MsgID_T msgID = Can_No_Msg;
    while (msgID == Can_No_Msg) {
      msgID = Can_MsgType();
      //if(msTicks % 100 == 0) Serial_Println("No CAN msg");
    }
    Can_FrontCanNodeWheelSpeed_Read(&rx_msg);

    tx_msg.front_left_wheel_speed = rx_msg.front_left_wheel_speed;
    tx_msg.front_right_wheel_speed = math(tx_msg.front_left_wheel_speed);

    Can_FrontCanNodeWheelSpeed_Write(&tx_msg);
    Serial_Println("Msg read");
    Serial_PrintlnNumber(msTicks, 10);
  }
}
