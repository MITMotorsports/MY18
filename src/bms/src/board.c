#include "board.h"

#include "cell_temperatures.h"
#include "error_handler.h"

#define TEST_HARDWARE

const uint32_t OscRateIn = 0;

volatile uint32_t msTicks;

// msTicks increment interrupt handler
void SysTick_Handler(void) {
  msTicks++;
}

void Board_Chip_Init(void) {
  SysTick_Config(Hertz2Ticks(1000));
}

void Board_BlockingDelay(uint32_t delayms) {
  uint32_t haltTime = msTicks + delayms;

  while (msTicks < haltTime) ;
}
