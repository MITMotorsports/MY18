#include "serial.h"

#include "sysinit.h"
#include "chip.h"

#define SERIAL_BAUDRATE 57600
#define CAN_BAUDRATE 500000

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);
  Can_Init(CAN_BAUDRATE);

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  while(1) {

  }
}
