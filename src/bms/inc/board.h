#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "can_raw.h"

#ifndef _BOARD_H_
#define _BOARD_H_

#define Hertz2Ticks(freq) SystemCoreClock / freq
void Board_Chip_Init(void);
void Board_GPIO_Init(void);
void Board_CAN_Init(uint32_t baudrate);
#endif