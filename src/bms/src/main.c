#include <string.h>
#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "../../../lib/lpc11cx4-library/evt_lib/inc/ltc6804.h"
#include "board.h"
#include "pack.h"
#include "state_types.h"
#include "config.h"

#define CAN_BAUD 500000

extern volatile uint32_t msTicks;

static BMS_PACK_STATUS_T pack_status;
static BMS_INPUT_T bms_input;

static uint32_t cell_voltages[MAX_NUM_MODULES*MAX_CELLS_PER_MODULE];
static int16_t cell_temperatures[MAX_NUM_MODULES*MAX_THERMISTORS_PER_MODULE];



void Init_BMS_Structs(void){

    bms_input.pack_status = &pack_status;

    pack_status.cell_voltages_mV = cell_voltages;
    pack_status.cell_temperatures_dC = cell_temperatures;
    pack_status.pack_cell_max_mV = 0;
    pack_status.pack_cell_min_mV = 0xFFFFFFFF;
    pack_status.pack_current_mA = 0;
    pack_status.pack_voltage_mV = 0;
    pack_status.max_cell_temp_dC = 0;

    pack_status.min_cell_temp_dC = -100;
    pack_status.avg_cell_temp_dC = 0;
    pack_status.min_cell_temp_position = 0;
    pack_status.max_cell_temp_position = 0;
}

void Process_Input(BMS_INPUT_T* bms_input){
    Board_CAN_ProcessInput(bms_input);


}

void Process_Output(/*bms output*/){

}
int main(void){
	Board_Chip_Init();
	Board_GPIO_Init();
	Board_CAN_Init(CAN_BAUD);
    while(1){
        Process_Input(&bms_input);
    }
	return 0;
}

