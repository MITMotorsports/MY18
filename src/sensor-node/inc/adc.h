#ifndef ADC_H
#define ADC_H

#include "chip.h"

#include "types.h"

#define STEERING_CHANNEL ADC_CH3

void ADC_Init(void);
uint16_t ADC_Read(ADC_CHANNEL_T channel);

#endif // ADC_H
