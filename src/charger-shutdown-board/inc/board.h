#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdlib.h>

#include "chip.h"

#include "gpio.h"
#include "uart.h"


extern volatile uint32_t msTicks;

#define Hertz2Ticks(freq) SystemCoreClock / freq

// Functions
void Board_Chip_Init(void);




#endif // ifndef _BOARD_H_
