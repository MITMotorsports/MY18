#include "ssm.h"

volatile uint32_t msTicks;

void SSM_Init(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output){
    state->curr_mode = BMS_SSM_MODE_INIT;
    state->init_state = BMS_INIT_OFF;

    output->read_eeprom_packconfig = false;
    output->check_packconfig_with_ltc = false;

    input->ltc_packconfig_check_done = false;
    input->eeprom_packconfig_read_done = false;

    Discharge_Init();
}

void Init_Step(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output){
    switch(state->init_state){
        case(BMS_INIT_OFF):
            Board_Println_BLOCKING("In INIT_OFF");
            output->read_eeprom_packconfig = true;
            state->init_state = BMS_INIT_READ_PACKCONFIG;
            input->eeprom_packconfig_read_done = false;
            break;
        case(BMS_INIT_READ_PACKCONFIG):
            Board_Println_BLOCKING("In INIT_READ_PACKCONFIG");
            if(input->eeprom_packconfig_read_done){
                output->read_eeprom_packconfig = false;
                output->check_packconfig_with_ltc = true;
                state->init_state = BMS_INIT_CHECK_PACKCONFIG;
                input->ltc_packconfig_check_done = false;
                input->eeprom_packconfig_read_done = false;
            }
            break;
        case(BMS_INIT_CHECK_PACKCONFIG):
            Board_Println_BLOCKING("In INIT_CHECK_PACKCONFIG");
            if(input->ltc_packconfig_check_done) {
                output->check_packconfig_with_ltc = false;
                state->init_state = BMS_INIT_DONE;
                input->ltc_packconfig_check_done = false;
            }
            break;
        case(BMS_INIT_DONE):
            //Board_Println_BLOCKING("In INIT_DONE");
            //state->curr_mode = BMS_SSM_MODE_STANDBY;
            break;
    }
}



bool Is_Valid_Jump(BMS_SSM_MODE_T mode1, BMS_SSM_MODE_T mode2) {

    if(mode1 == BMS_SSM_MODE_STANDBY && mode2 == BMS_SSM_MODE_CHARGE) {
        return true;
    } else if(mode1 == BMS_SSM_MODE_STANDBY && mode2 == BMS_SSM_MODE_BALANCE) {
        return true;
    } else if(mode1 == BMS_SSM_MODE_STANDBY && mode2 == BMS_SSM_MODE_DISCHARGE) {
        return true;

    } else if(mode1 == BMS_SSM_MODE_CHARGE && mode2 == BMS_SSM_MODE_STANDBY) {
        return true;
    } else if(mode1 == BMS_SSM_MODE_BALANCE && mode2 == BMS_SSM_MODE_STANDBY) {
        return true;
    } else if(mode1 == BMS_SSM_MODE_DISCHARGE && mode2 == BMS_SSM_MODE_STANDBY) {
        return true;
    } else if(mode1 == BMS_SSM_MODE_BALANCE && mode2 == BMS_SSM_MODE_STANDBY) {
        return true;

    } else if(mode1 == BMS_SSM_MODE_BALANCE && mode2 == BMS_SSM_MODE_CHARGE) {
        return true;
    } else if(mode1 == BMS_SSM_MODE_CHARGE && mode2 == BMS_SSM_MODE_BALANCE) {
        return true;
    }
    return false;
}

bool Is_State_Done(BMS_STATE_T *state) {
    switch(state->curr_mode){
        case BMS_SSM_MODE_DISCHARGE:
            return true;
    }
    return false;
}

void SSM_Step(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output){
    switch(state->curr_mode){
        case BMS_SSM_MODE_STANDBY:
            break;
        case BMS_SSM_MODE_INIT:
            Init_Step(input,state,output);
            break;
        case BMS_SSM_MODE_CHARGE:
            break;
        case BMS_SSM_MODE_DISCHARGE:
            break;
        case BMS_SSM_MODE_BALANCE:
            break;
    }
}














