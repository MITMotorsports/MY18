#include "chip.h"
#include "CANlib.h"
#include <string.h>

#define LED0 2, 8
#define LED1 2, 10

const uint32_t OscRateIn = 12000000;
volatile uint32_t millis = 0;

void SysTick_Handler(void) {
  millis++;
}

static void LED_Init(uint8_t port, uint8_t pin) {
  Chip_GPIO_WriteDirBit(LPC_GPIO, port, pin, true);
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, false);
}

static void LED_Write(uint8_t port, uint8_t pin, uint8_t val) {
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, val);
}

int main(void) {
  SystemCoreClockUpdate();

  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Error
    while (1);
  }

  /// UART
	// RXD
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));
	// TXD
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));

  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, 9600);
  Chip_UART_ConfigData(LPC_USART,
                       (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT |
                        UART_LCR_PARITY_DIS));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);

  /// CAN
  Can_Init(500000);

  UART_Print("Started up!\n");

  LED_Init(LED0);
  LED_Init(LED1);

  LED_Write(LED0, true);

  // Definition of frame pasted here for convenience
  // typedef struct {
  //   uint32_t id;
  //   uint8_t len;
  //   uint8_t data[8];
  //   bool extended;
  // } Frame;

  Frame rx_frame;

  while (1) {
    Can_RawRead(&rx_frame);
  }
}
