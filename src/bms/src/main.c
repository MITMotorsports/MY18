#include <string.h>
#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "../../../lib/lpc11cx4-library/evt_lib/inc/ltc6804.h"
#include "board.h"


#define CAN_BAUD 500000

extern volatile uint32_t msTicks;


int main(void){
	Board_Chip_Init();
	Board_GPIO_Init();
	Board_CAN_Init(CAN_BAUD);
    //Can_Init(CAN_BAUD);

	return 0;
}

