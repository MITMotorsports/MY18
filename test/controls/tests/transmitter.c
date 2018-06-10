#include "chip.h"
#include "serial.h"
#include "sysinit.h"
#include "Can_Library.h"
#include "adc.h"

#define BAUDRATE 57600
#define CAN_BAUDRATE 500000

#define ITERS 100
#define PERIOD 5

uint32_t log[ITERS][2]; // Store start and end tick for each iteration

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

void SysTick_Handler(void) {
  msTicks++;
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(BAUDRATE);
  Can_Init(CAN_BAUDRATE);

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  ADC_Init();

  int i;
  uint32_t next_iter_time = msTicks;
  //for(i = 0; i < ITERS; i++) {
  while(1) {
    while(next_iter_time >  msTicks);
    next_iter_time += PERIOD;
    // Debug prints
    //Serial_Print("Start tick: ");
    //Serial_PrintlnNumber(msTicks, 10);

    log[i][0] = msTicks;

    uint16_t adc_data = 0;

    Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
    // Waiting for A/D conversion complete
    while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH0, ADC_DR_DONE_STAT)) {}
    // Read ADC value
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &adc_data);

    // Debug prints
    // Serial_Print("ADC read: ");
    // Serial_PrintlnNumber(adc_data, 10);

    Can_FrontCanNodeWheelSpeed_T tx_msg;
    Can_FrontCanNodeWheelSpeed_T rx_msg;

    tx_msg.front_left_wheel_speed = adc_data;
    tx_msg.front_right_wheel_speed = 0;
    Can_FrontCanNodeWheelSpeed_Write(&tx_msg);
    Can_MsgID_T msgID = Can_No_Msg;
    while (msgID == Can_No_Msg) {
      msgID = Can_MsgType();
      //if(msTicks % 100 == 0) Serial_Println("No CAN msg");
    }
    Can_FrontCanNodeWheelSpeed_Read(&rx_msg);

    // Debug prints
    /*Serial_Print("End tick: ");
    Serial_PrintlnNumber(msTicks, 10);
    Serial_Print("Data from CAN msg: ");
    Serial_PrintNumber(rx_msg.front_left_wheel_speed, 10);
    Serial_Print(", ");
    Serial_PrintlnNumber(rx_msg.front_right_wheel_speed, 10);
    Serial_Println("");*/

    log[i][1] = msTicks;
  }
  int j;
  for(j = 0; j < ITERS; j++) {
    Serial_PrintNumber(j, 10);
    Serial_Print(",");
    Serial_PrintNumber(log[j][0], 10);
    Serial_Print(",");
    Serial_PrintlnNumber(log[j][1], 10);
  }

  return 0;
}
