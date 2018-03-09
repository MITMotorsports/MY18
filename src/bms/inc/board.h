#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdlib.h>

#include "chip.h"

#include "canmgr.h"
#include "gpio.h"
#include "uart.h"
#include "isospi.h"

#include "console.h"

extern volatile uint32_t msTicks;

#define Hertz2Ticks(freq) SystemCoreClock / freq

// Functions
void Board_Chip_Init(void);

// -- Delay
void Board_BlockingDelay(uint32_t delayms);


// -- Get state machine mode request from console
void Board_GetModeRequest(const CONSOLE_OUTPUT_T *console_output,
                          BMS_INPUT_T            *bms_input);

#endif // ifndef _BOARD_H_
