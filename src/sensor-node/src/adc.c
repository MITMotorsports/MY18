#include "adc.h"

static ADC_CLOCK_SETUP_T adc_setup;
static const uint8_t int_ch[] = {ADC_CH2, ADC_CH3, ADC_CH4, ADC_CH5};

#define init_ADC_by_name(name)                                                 \
  Chip_IOCON_PinMuxSet(LPC_IOCON, name ## _PIN, name ## _PIN_CFG);             \
  Chip_ADC_EnableChannel(LPC_ADC, name, ENABLE);

void Init_Internal_ADC(void) {
	Chip_ADC_Init(LPC_ADC, &adc_setup);

  // shock pots
  init_ADC_by_name(ADC_CH2); // rear left$
  init_ADC_by_name(ADC_CH3); // rear right $
  init_ADC_by_name(ADC_CH4); // front left $
  init_ADC_by_name(ADC_CH5); // front right $

  Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
}

#undef init_ADC_by_name

uint16_t Read_Internal_ADC(ADC_CHANNEL_T channel) {
  uint16_t result = 0;
  Chip_ADC_ReadValue(LPC_ADC, channel, &result);
  return result;
}

void Read_Internal_ADC_Range(uint16_t* int_adc_data, unsigned start, unsigned stop, unsigned step) {
  for (unsigned ch = start; ch < stop; ch += step) {
  	int_adc_data[ch] = Read_Internal_ADC(int_ch[ch]);
  }
}