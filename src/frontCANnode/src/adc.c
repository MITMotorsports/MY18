// Other than names, copied from MY17
#include "adc.h"

#include <stdint.h>
#include <string.h>

#include "serial.h"

static ADC_CLOCK_SETUP_T adc_setup;

void ADC_Init(void) {
  const uint32_t FRONT_BRAKEIN_CONFIG = IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN;

  Chip_ADC_Init(LPC_ADC, &adc_setup);

  // Steering
  Chip_IOCON_PinMuxSet(LPC_IOCON, STEERING_PIN, STEERING_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, STEERING_CHANNEL, ENABLE);
  // Left throttle
  Chip_IOCON_PinMuxSet(LPC_IOCON, LEFT_THROTTLE_POT_PIN, LEFT_THROTTLE_POT_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, LEFT_THROTTLE_POT_CHANNEL, ENABLE);
  // Right throttle
  Chip_IOCON_PinMuxSet(LPC_IOCON, RIGHT_THROTTLE_POT_PIN, RIGHT_THROTTLE_POT_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, RIGHT_THROTTLE_POT_CHANNEL, ENABLE);
  // Front brake
  Chip_IOCON_PinMuxSet(LPC_IOCON, FRONT_BRAKE_PIN, FRONT_BRAKE_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, FRONT_BRAKE_CHANNEL, ENABLE);
  // Rear brake
  Chip_IOCON_PinMuxSet(LPC_IOCON, REAR_BRAKE_PIN, REAR_BRAKE_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, REAR_BRAKE_CHANNEL, ENABLE);

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
