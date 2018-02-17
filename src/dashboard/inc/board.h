#ifndef _BOARD_H_
#define _BOARD_H_

#include "chip.h"
#include "pins.h"

void Board_Chip_Init(void);
void Board_GPIO_Init(void);

void Pin_Init(uint8_t port, uint8_t pin);

void Pin_Write(uint8_t port, uint8_t pin, uint8_t val);
bool Pin_Read(uint8_t port, uint8_t pin);

void Delay(uint32_t dly);

#endif
