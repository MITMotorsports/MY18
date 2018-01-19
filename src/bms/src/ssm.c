#include "ssm.h"
#include "state_types.h"

volatile uint32_t msTicks;

void SSM_Init(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output){
    state->curr_mode = BMS_SSM_MODE_INIT;
    state->init_state = BMS_INIT_OFF;

    output->read_eeprom_packconfig = false;
    output->check_packconfig_with_ltc = false;

    input->ltc_packconfig_check_done = false;
    input->eeprom_packconfig_read_done = false;
}














