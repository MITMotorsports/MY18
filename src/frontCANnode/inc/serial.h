#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void Serial_Init(uint32_t baudrate);

uint32_t Serial_Print(const char *str);

void Serial_Print_Void(const char *str);

uint32_t Serial_Println(const char *str);

uint32_t Serial_PrintNumber(uint32_t n, uint32_t base);

uint32_t Serial_PrintlnNumber(uint32_t n, uint32_t base);

#endif //SERIAL_H
