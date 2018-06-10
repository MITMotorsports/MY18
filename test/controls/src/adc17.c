#include "adc17.h"

#include <stdint.h>
#include <string.h>

#include "serial.h"

#define ACCEL_1_LOWER_BOUND 105
#define ACCEL_1_UPPER_BOUND 645
#define ACCEL_2_LOWER_BOUND 71
#define ACCEL_2_UPPER_BOUND 340

static ADC_CLOCK_SETUP_T adc_setup;

uint16_t linear_transfer_fn(uint32_t reading, uint16_t desired_width, uint16_t lower_bound, uint16_t upper_bound);
uint16_t transform1(uint32_t accel_1_raw);
uint16_t transform2(uint32_t accel_2_raw);

void ADC_Init(void) {
  const uint32_t ADC_PIN_CONFIG = IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN;

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
  // TODO re-enable ones for wheel speed
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

uint16_t linear_transfer_fn(uint32_t reading, uint16_t desired_width, uint16_t lower_bound, uint16_t upper_bound) {
  // Mostly copied from MY17

  // Ensure reading is within expected range
  if (lower_bound > reading) reading = lower_bound;
  if (upper_bound < reading) reading = upper_bound;

  // Make reading between 0 and diff
  const uint16_t diff = upper_bound - lower_bound;
  reading = reading - lower_bound;

  // Now scale from [0:diff] to [0:desired_width].
  // Note: it's critical that reading be a 32 bit int because otherwise this line will cause overflow!
  reading = reading * desired_width;
  reading = reading / diff;

  // This cast is safe because we have asserted reading is in range [0:desired_width] in previous lines
  uint16_t short_val = (uint16_t)(reading);
  return short_val;
}

uint16_t transform1(uint32_t accel_1_raw) {
  // Scale to between 0 and 1000
  return linear_transfer_fn(accel_1_raw, 1000, ACCEL_1_LOWER_BOUND, ACCEL_1_UPPER_BOUND);
}

uint16_t transform2(uint32_t accel_2_raw) {
  // Scale to between 0 and 1000
  return linear_transfer_fn(accel_2_raw, 1000, ACCEL_2_LOWER_BOUND, ACCEL_2_UPPER_BOUND);
}
