#include "main.h"

static Orient_T orient;

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

uint16_t ext_adc_data[8]; //external adc data
uint16_t int_adc_data[4];

void SysTick_Handler(void) {
  msTicks++;
}

//15 0
#define SERIAL_BAUDRATE 57600

#define LED_PIN 5

/* Tx buffer */


static void LED_Config(void) {
	Chip_GPIO_WriteDirBit(LPC_GPIO, 2, LED_PIN, true);

}

static void LED_On(void) {
	Chip_GPIO_SetPinState(LPC_GPIO, 2, LED_PIN, true);
	Chip_UART_SendBlocking(LPC_USART, "LED_ON", 6);
}

static void LED_Off(void) {
	Chip_GPIO_SetPinState(LPC_GPIO, 2, LED_PIN, false);
}

void Serial_Init(uint32_t baudrate) {
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* TXD */

  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, baudrate);
  // Configure data width, parity, and stop bits
  Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);
}


int main(void)
{
	SystemCoreClockUpdate();
  	Serial_Init(SERIAL_BAUDRATE);

	if (SysTick_Config (SystemCoreClock / 1000)) {
		//Error
		while(1);
	}


	/* LED Initialization */
	GPIO_Init();
	LED_Config();
	LED_On();

	SSP_Init();
  	ADC_Init();

	/* SSP initialization 	*/
  	I2C_Init();
  	Accel_Init();

	LED_On();

	while (1) {
		// println("looping");

		// SPI_Read_ADC(ext_adc_data);
		// for (int i = 0; i < 2; i++) {
		// 	print("EXT ADC DATA : ");
		// 	printNum(ext_adc_data[i],10);
		// 	println("");
		// }
		// Internal_Read_ADC(int_adc_data);
		// for (int i = 0; i < 4; i++) {
		// 	print("INT ADC DATA : ");
		// 	printNum(int_adc_data[i],10);
		// 	println("");
		// }
		Read_Axes(&orient);
		// can_transmit(ext_adc_data, int_adc_data);

	}
	return 0;
}
