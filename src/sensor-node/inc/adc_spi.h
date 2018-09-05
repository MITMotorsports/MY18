#ifndef _ADC_SPI_H_
#define _ADC_SPI_H_

#include "chip.h"

// SSP constants
#define SSP_MODE_TEST              1  // 1: Master, 0: Slave
#define BUFFER_SIZE                (0x100)
#define SSP_DATA_BITS              (SSP_BITS_16)
#define SSP_DATA_BIT_NUM(databits) (databits + 1)
#define SSP_DATA_BYTES(databits)   (((databits) > SSP_BITS_8) ? 2 : 1)
#define SSP_LO_BYTE_MSK(databits)  ((SSP_DATA_BYTES(databits) > 1) ? 0xFF :    \
															      (0xFF >> (8 - SSP_DATA_BIT_NUM(databits))))
#define SSP_HI_BYTE_MSK(databits)  ((SSP_DATA_BYTES(databits) > 1) ? \
																	  (0xFF >> (16 - SSP_DATA_BIT_NUM(databits))) : 0)
#define LPC_SSP           LPC_SSP0
#define SSP_IRQ           SSP0_IRQn
#define SSPIRQHANDLER     SSP0_IRQHandler

#define CS 2, 11
#define V_A 5

void Init_SPI_ADC(void);

void Read_SPI_ADC(unsigned ch);
void Read_SPI_ADC_Range(uint16_t* ext_adc_data, unsigned start, unsigned stop, unsigned step);

#endif
