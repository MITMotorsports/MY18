#include "uart.h"


// UART Initialization
void UART_IRQHandler(void) {
  Chip_UART_IRQRBHandler(LPC_USART, &uart_rx_ring, &uart_tx_ring);
}

void Board_UART_Init(uint32_t baudRateHz) {
  RingBuffer_Init(&uart_rx_ring, _uart_rx_ring, sizeof(uint8_t),
                  UART_BUFFER_SIZE);
  RingBuffer_Flush(&uart_rx_ring);
  RingBuffer_Init(&uart_tx_ring, _uart_tx_ring, sizeof(uint8_t),
                  UART_BUFFER_SIZE);
  RingBuffer_Flush(&uart_tx_ring);

  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); /*
                                                                                      RXD
                                                                                    */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); /*
                                                                                      TXD
                                                                                    */

  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaudFDR(LPC_USART, baudRateHz);
  Chip_UART_ConfigData(LPC_USART,
                       (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT |
                        UART_LCR_PARITY_DIS));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);

  Chip_UART_IntEnable(LPC_USART, UART_IER_RBRINT);
  NVIC_ClearPendingIRQ(UART0_IRQn);
  NVIC_EnableIRQ(UART0_IRQn);
}

uint32_t Board_Print(const char *str) {
  return Chip_UART_SendRB(LPC_USART, &uart_tx_ring, str, strlen(str));
}

uint32_t Board_Println(const char *str) {
  uint32_t count = Board_Print(str);

  return count + Board_Print("\r\n");
}

uint32_t Board_PrintNum_BLOCKING(uint32_t a, uint8_t base) {
  itoa(a, str, base);
  return Board_Print_BLOCKING(str);
}

uint32_t Board_PrintNum(uint32_t a, uint8_t base) {
  itoa(a, str, base);
  return Board_Print(str);
}

uint32_t Board_Write(const char *str, uint32_t count) {
  return Chip_UART_SendRB(LPC_USART, &uart_tx_ring, str, count);
}

uint32_t Board_Read(char *charBuffer, uint32_t length) {
  return Chip_UART_ReadRB(LPC_USART, &uart_rx_ring, charBuffer, length);
}

uint32_t Board_Print_BLOCKING(const char *str) {
  return Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}

uint32_t Board_Println_BLOCKING(const char *str) {
  uint32_t count = Board_Print_BLOCKING(str);

  return count + Board_Print_BLOCKING("\r\n");
}
