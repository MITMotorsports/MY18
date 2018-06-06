#ifndef ADC_H
#define ADC_H

#include "chip.h"

#define CHANNEL ADC_CH0
#define UNUSED_1 ADC_CH1
#define UNUSED_2 ADC_CH2
#define UNUSED_3 ADC_CH3
#define UNUSED_4 ADC_CH4
#define UNUSED_5 ADC_CH5
#define UNUSED_6 ADC_CH6
#define UNUSED_7 ADC_CH7

#define ADC_PIN IOCON_PIO0_11
#define CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)

void ADC_Init(void);
uint16_t ADC_Read(void);

#endif // ADC_H
