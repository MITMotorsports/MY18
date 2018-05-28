#ifndef _UART_H__
#define _UART_H__

#include "chip.h"

void print(const char *str); 
void println(const char *str);
void printNum(uint32_t num, uint8_t base);

#endif