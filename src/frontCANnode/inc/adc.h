#ifndef ADC_H
#define ADC_H

#include "chip.h"

#include "types.h"

#define STEERING_CHANNEL ADC_CH0
#define ACCEL_1_CHANNEL ADC_CH2
#define ACCEL_2_CHANNEL ADC_CH3
#define BRAKE_1_CHANNEL ADC_CH4
#define BRAKE_2_CHANNEL ADC_CH5

void ADC_Init(void);
uint16_t ADC_Read(ADC_CHANNEL_T channel);

#endif // ADC_H
