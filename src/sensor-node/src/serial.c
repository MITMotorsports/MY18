#include "serial.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "chip.h"

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

uint32_t Serial_Print(const char *str) {
  return Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}

void Serial_Print_Void(const char *str) {
  Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}
uint32_t Serial_Println(const char *str) {
  uint32_t count = Serial_Print(str);
  return count + Serial_Print("\r\n");
}

uint32_t Serial_PrintNumber(uint32_t n, uint32_t base) {
  const uint8_t max_digits = 33;
  char n_str[max_digits];
  itoa(n, n_str, base);
  return Serial_Print(n_str);
}

uint32_t Serial_PrintlnNumber(uint32_t n, uint32_t base) {
  uint32_t count = Serial_PrintNumber(n, base);
  return count + Serial_Print("\r\n");
}
