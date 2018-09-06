#include "chip.h"

const uint32_t OscRateIn = 12000000;

#define LED0 2, 8
#define LED1 0, 6
#define LED2 0, 7
#define LED3 2, 9

// More info about volatile here:
// https://en.wikipedia.org/wiki/Volatile_(computer_programming)#In_C_and_C++

// This variable counts up the milliseconds passed since boot.
volatile uint32_t millis = 0;

void SysTick_Handler(void) {
  millis++;
}

static void Delay(uint32_t millis) {
  // Write a delay function!
  // Currently this delays for exactly 0.00 microseconds.
}

static void GPIO_Config(void) {
  Chip_GPIO_Init(LPC_GPIO);
}

static void LED_Init(uint8_t port, uint8_t pin) {
  Chip_GPIO_WriteDirBit(LPC_GPIO, port, pin, true);
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, false);
}

static void LED_Write(uint8_t port, uint8_t pin, uint8_t val) {
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, val);
}

int main(void) {
  SystemCoreClockUpdate();


  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Error
    while (1);
  }

  LED_Init(LED0);
  LED_Init(LED1);
  LED_Init(LED2);
  LED_Init(LED3);

  GPIO_Config();


  while (1) {}
}
