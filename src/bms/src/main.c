#include <string.h>
#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "../../../lib/lpc11cx4-library/evt_lib/inc/ltc6804.h"
#include "board.h"


#define CAN_BAUD 500000

extern volatile uint32_t msTicks;

void Process_Input(/*bmsInput*/){
    Board_CAN_ProcessInput();
    //mode requests done through can ^^


}

void Process_Output(/*bms output*/){

}
int main(void){
	Board_Chip_Init();
	Board_GPIO_Init();
	Board_CAN_Init(CAN_BAUD);
    //Can_Init(CAN_BAUD);
    while(1){
        Process_Input();
    }
	return 0;
}

