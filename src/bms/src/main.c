#include <string.h>
#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "../../../lib/lpc11cx4-library/evt_lib/inc/ltc6804.h"
#include "board.h"


extern volatile uint32_t msTicks;


int main(void){
	Board_Chip_Init();
	return 0;
}

