#include "chip.h"
#include "serial.h"
#include "sysinit.h"
#include "adc.h"

#define BAUDRATE 57600

#define OUTPUT_PIN 1, 0
#define GPIO_CONFIG (IOCON_FUNC1)

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

#define PERIOD 5

static ADC_CLOCK_SETUP_T adc_setup;

void SysTick_Handler(void) {
  msTicks++;
}

void GPIO_Init(void) {
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, GPIO_CONFIG);
  Chip_GPIO_Init(LPC_GPIO);
  // Configure pin as outputs
  Chip_GPIO_WriteDirBit(LPC_GPIO, OUTPUT_PIN, true);
  Chip_GPIO_SetPinState(LPC_GPIO, OUTPUT_PIN, false);
}

void set_gpio(bool state) {
  Chip_GPIO_SetPinState(LPC_GPIO, OUTPUT_PIN, state);
}

int main(void) {
  SystemCoreClockUpdate();

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  Serial_Init(BAUDRATE);
  ADC_Init();
  GPIO_Init();

  Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PIN, CONFIG);

  uint16_t setpoint = 542;
  uint16_t delta = 20;
  uint16_t hysteresis_high = setpoint + delta;
  uint16_t hysteresis_low = setpoint - delta;

  uint64_t sum_vals = 0;

  uint16_t num_vals = 0;

  bool last_state = false;
  bool state = false;

  uint32_t next_iter_time = msTicks;
  while(1) {
    while(next_iter_time > msTicks);
    next_iter_time += PERIOD;
    uint16_t adc_data;

    adc_data = ADC_Read();
    /*sum_vals += adc_data;
    num_vals++;*/

    bool state;
    if (last_state) {
      state = adc_data < hysteresis_high;
    } else {
      state = adc_data < hysteresis_low;
    }
    set_gpio(state);
    last_state = state;

    if (msTicks % 100 == 0) {
      /*Serial_Print("GPIO state: ");
      Serial_PrintNumber(state, 10);
      Serial_Print(", ADC val: ");
      Serial_PrintlnNumber(adc_data, 10);/*
      Serial_Print(", sum of values: ");
      Serial_PrintNumber(sum_vals, 10);
      Serial_Print(", num of values: ");
      Serial_PrintNumber(num_vals, 10);
      Serial_Print(", average ADC val: ");
      Serial_PrintlnNumber(sum_vals/num_vals, 10);*/
    }
  }
}
