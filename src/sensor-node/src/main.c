#include "serial.h"
#include "types.h"
#include "input.h"
#include "output.h"
#include "adc.h"

#include "sysinit.h"
#include "chip.h"

#include "CANlib.h"

#define SERIAL_BAUDRATE 57600
#define CAN_BAUDRATE 500000

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

static ADC_Input_T adc;

Input_T input;

void SysTick_Handler(void) {
  msTicks++;
}

void initialize_input(void) {
  adc.steering_pot = 0;

  input.adc = &adc;

  input.msTicks = 0;
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);

  init_can0_can_node();

  ADC_Init();

  initialize_input();
  Serial_Println("Started up!");
  Serial_Println("Currently running "HASH);
  Serial_Println("Flashed by: "AUTHOR);

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  Frame raw_msg = {};
  uint32_t last_tick = 0;

  while(1) {
    Can_RawRead(&raw_msg);

    input.msTicks = msTicks;
    Input_fill_input();
    Output_process_output();
    if (msTicks - last_tick > 500) {
     Serial_PrintlnNumber(input.adc->steering_pot, 10);
    }
  }
}
