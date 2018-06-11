#ifndef _ADC_H_
#define _ADC_H_

#include "chip.h"
#include "uart.h"

void ADC_Init(void);
uint16_t Read_ADC(ADC_CHANNEL_T channel);

#endif
