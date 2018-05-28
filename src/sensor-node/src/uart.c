#include "uart.h"

static char* num_buffer[100];

void print(const char *str) {
	Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}

void println(const char *str) {
	print(str); print("\r\n");
}

void printNum(uint32_t num, uint8_t base) {
	itoa(num, num_buffer, base); print(num_buffer);
}
