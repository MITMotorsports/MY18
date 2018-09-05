#define _UART_H_
#ifdef _UART_H_

#include "chip.h"

// Defines
#define UART_BUFFER_SIZE 100

// Variables
static char str[10];

static RINGBUFF_T uart_rx_ring;
static uint8_t    _uart_rx_ring[UART_BUFFER_SIZE];
static RINGBUFF_T uart_tx_ring;
static uint8_t    _uart_tx_ring[UART_BUFFER_SIZE];

// Functions
// - Initialize
void     Board_UART_Init(uint32_t baudRateHz);

// - Non-Blocking
uint32_t Board_Print(const char *str);

uint32_t Board_Println(const char *str);

// - Blocking
uint32_t Board_Print_BLOCKING(const char *str);

uint32_t Board_Println_BLOCKING(const char *str);

uint32_t Board_PrintNum_BLOCKING(uint32_t num, uint8_t  base);
uint32_t Board_PrintNum(uint32_t num, uint8_t  base);

// - Read/Write from UART buffer
uint32_t Board_Write(const char *str,
                     uint32_t    count);

uint32_t Board_Read(char    *charBuffer,
                    uint32_t length);

#endif // ifdef _UART_H_
