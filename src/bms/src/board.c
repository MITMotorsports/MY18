#include "../inc/board.h"
#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "../../../lib/lpc11cx4-library/evt_lib/inc/ltc6804.h"
//#include "pins.h"

volatile uint32_t msTicks;

void SysTick_Handler(void) {
	msTicks++;
}

void Board_Chip_Init(void) {
	SysTick_Config(Hertz2Ticks(1000));
}