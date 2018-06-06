#include "adc.h"

static ADC_CLOCK_SETUP_T adc_setup;

void ADC_Init() {
  Chip_ADC_Init(LPC_ADC, &adc_setup);

  Chip_ADC_EnableChannel(LPC_ADC, CHANNEL, ENABLE);

  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_1, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_2, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_3, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_4, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_5, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_6, DISABLE);
  Chip_ADC_EnableChannel(LPC_ADC, UNUSED_7, DISABLE);
}

uint16_t ADC_Read() {
  uint16_t adc_data = 0;

  Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
  // Waiting for A/D conversion complete
  while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH0, ADC_DR_DONE_STAT)) {}
  // Read ADC value
  Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &adc_data);

  return adc_data;
}
