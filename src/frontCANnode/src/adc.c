#include "adc.h"

#include <stdint.h>
#include <string.h>

#include "serial.h"

#define UNUSED_1_CHANNEL ADC_CH1
#define UNUSED_2_CHANNEL ADC_CH6
#define UNUSED_3_CHANNEL ADC_CH7

#define STEERING_PIN IOCON_PIO0_11
#define ACCEL_1_PIN IOCON_PIO1_1
#define ACCEL_2_PIN IOCON_PIO1_2
#define BRAKE_1_PIN IOCON_PIO1_3
#define BRAKE_2_PIN IOCON_PIO1_4

// FUNC 2 is ADC for all pins here except for PIO1_4
#define ACCEL_1_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define ACCEL_2_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define BRAKE_1_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define BRAKE_2_PIN_CONFIG (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define STEERING_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)

static ADC_CLOCK_SETUP_T adc_setup;

void ADC_Init(void) {
  Chip_ADC_Init(LPC_ADC, &adc_setup);

  // Steering / AD0
  Chip_IOCON_PinMuxSet(LPC_IOCON, STEERING_PIN, STEERING_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, STEERING_CHANNEL, ENABLE);
  // Accel 1 / AD2
  Chip_IOCON_PinMuxSet(LPC_IOCON, ACCEL_1_PIN, ACCEL_1_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, ACCEL_1_CHANNEL, ENABLE);
  // Accel 2 / AD3
  Chip_IOCON_PinMuxSet(LPC_IOCON, ACCEL_2_PIN, ACCEL_2_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, ACCEL_2_CHANNEL, ENABLE);
  // Brake 1 / AD4
  Chip_IOCON_PinMuxSet(LPC_IOCON, BRAKE_1_PIN, BRAKE_1_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, BRAKE_1_CHANNEL, ENABLE);
  // Brake 2 / AD5
  Chip_IOCON_PinMuxSet(LPC_IOCON, BRAKE_2_PIN, BRAKE_2_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, BRAKE_2_CHANNEL, ENABLE);

  // Disable all other channels
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_1_CHANNEL, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_2_CHANNEL, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_3_CHANNEL, DISABLE);

  // Enable burst
  Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
  Chip_ADC_SetStartMode(LPC_ADC, ADC_NO_START, ADC_TRIGGERMODE_RISING);
}

uint16_t ADC_Read(ADC_CHANNEL_T channel) {
  uint16_t result = 0;
  Chip_ADC_ReadValue(LPC_ADC, channel, &result);
  return result;
}
