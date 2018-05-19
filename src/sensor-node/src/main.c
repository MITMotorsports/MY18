#include "chip.h"

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

void SysTick_Handler(void) {
  msTicks++;
}

//15 0
#define SERIAL_BAUDRATE 57600

#define SSP_MODE_TEST       1	/*1: Master, 0: Slave */
#define BUFFER_SIZE                         (0x100)
#define SSP_DATA_BITS                       (SSP_BITS_16)
#define SSP_DATA_BIT_NUM(databits)          (databits + 1)
#define SSP_DATA_BYTES(databits)            (((databits) > SSP_BITS_8) ? 2 : 1)
#define SSP_LO_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? 0xFF : (0xFF >> \
																					  (8 - SSP_DATA_BIT_NUM(databits))))
#define SSP_HI_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? (0xFF >> \
																			   (16 - SSP_DATA_BIT_NUM(databits))) : 0)
#define LPC_SSP           LPC_SSP0
#define SSP_IRQ           SSP0_IRQn
#define SSPIRQHANDLER     SSP0_IRQHandler


#define LED_PIN 5


#define CS 2, 11
#define V_A 5

/* Tx buffer */
static uint8_t Tx_Buf[BUFFER_SIZE];

/* Rx buffer */
static uint16_t Rx_Buf[BUFFER_SIZE];

static SSP_ConfigFormat ssp_format;
static Chip_SSP_DATA_SETUP_T xf_setup;
static volatile uint8_t  isXferCompleted = 0;

static char* num_buffer[100];

#define print(str) {Chip_UART_SendBlocking(LPC_USART, str, strlen(str));}
#define println(str) {print(str); print("\r\n");}
#define printNum(num, base) {itoa(num, num_buffer, base); print(num_buffer)}

static void Init_SSP_PinMux(void) {
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MISO0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MOSI0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* SSEL0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_10, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SCK0 */
	Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO2_11);
}

static void Buffer_Init(void)
{
	uint16_t i;
	uint8_t ch = 0;
	for (i = 0; i < BUFFER_SIZE; i++) {
		Tx_Buf[i] = 0x1;
		Rx_Buf[i] = 0x0;
	}
}

static void GPIO_Config(void) {
	Chip_GPIO_Init(LPC_GPIO);

}

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

/**
 * @brief	Main routine for SSP example
 * @return	Nothing
 */
int main(void)
{
	SystemCoreClockUpdate();
  	Serial_Init(SERIAL_BAUDRATE);

	if (SysTick_Config (SystemCoreClock / 1000)) {
		//Error
		while(1);
	}

  // Chip_UART_SendBlocking(LPC_USART, "Y0YOyo", 6);

	/* LED Initialization */
	GPIO_Config();
	LED_Config();
	LED_On();



	/* SSP initialization */
	Init_SSP_PinMux();
	Chip_SSP_Init(LPC_SSP);
	Chip_SSP_SetBitRate(LPC_SSP, 30000);
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_SetMaster(LPC_SSP, SSP_MODE_TEST);
	Chip_SSP_Enable(LPC_SSP);
	Buffer_Init();
	LED_On();
	int i;

	while (1) {
		// println("looping");

		//4.730 volts == 2038
		//4.571 volts == 1970
		//2.879 volts == 1240
		//3.58 volts == 1542
		//2.127 volts = 916
		// 1.337 volts == 575
		//0.826 volts == 355
		//0 = 0
		Chip_SSP_WriteFrames_Blocking(LPC_SSP, Tx_Buf, BUFFER_SIZE);
		Chip_GPIO_SetPinState(LPC_GPIO, CS, true);
		Chip_SSP_ReadFrames_Blocking(LPC_SSP, Rx_Buf, BUFFER_SIZE);
		// for (i = 0; i < BUFFER_SIZE; i++) {
		// 	printNum(i,10);
		// 	print(",");
		// 	printNum(Rx_Buf[i],10);
		// 	println("");
		// }
		printNum(Rx_Buf[0],10);
		println("");
	}
	return 0;
}
