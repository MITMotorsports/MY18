#include "main.h"

static Orient_T orient;

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

uint16_t ext_adc_data[8]; //external adc data
uint16_t int_adc_data[4];

void SysTick_Handler(void) {
  msTicks++;
}

int main(void) {
	SystemCoreClockUpdate();
  if (SysTick_Config (SystemCoreClock / 1000)) while(1);  // Hang on error

  Board_UART_Init(57600);

	GPIO_Init();

	Init_SPI_ADC();
  Init_Internal_ADC();

  // I2C_Init();
  // Accel_Init();

	while (1) {
    Board_Print("CH2: ");
    Board_PrintNum(Read_Internal_ADC(ADC_CH2), 10);
    Board_Println("");
    Board_Print("CH3: ");
    Board_PrintNum(Read_Internal_ADC(ADC_CH3), 10);
    Board_Println("");
    Board_Print("CH4: ");
    Board_PrintNum(Read_Internal_ADC(ADC_CH4), 10);
    Board_Println("");
    Board_Print("CH5: ");
    Board_PrintNum(Read_Internal_ADC(ADC_CH5), 10);
    Board_Println("");

    uint32_t future = msTicks + 100;
    while (msTicks < future);
		//Read_Axes(&orient);
		// can_transmit(ext_adc_data, int_adc_data);
	}

	return 0;
}
