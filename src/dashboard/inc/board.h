#ifndef _BOARD_H_
#define _BOARD_H_

#include "CANlib.h"

#include "chip.h"
#include "pins.h"

extern volatile uint32_t msTicks;

void Board_Chip_Init(void);
void Board_GPIO_Init(void);

void Pin_Init(uint8_t port, uint8_t pin);

void Pin_Write(uint8_t port, uint8_t pin, uint8_t val);
bool Pin_Read(uint8_t port, uint8_t pin);

uint16_t ADC_Read(ADC_CHANNEL_T ch);

void Delay(uint32_t dly);

void handle_can_error(Can_ErrorID_T error);

#define CAN_Init() init_can0_dash()

#endif
