#define _CANMGR_H
#ifdef _CANMGR_H

#include <stdint.h>

#include <stdint.h>

#include "CANlib.h"
#include "state_types.h"
#include "error_handler.h"
#include "config.h"

// Variables
static Frame can_input;

// Functions
// - Initialize
void Board_CAN_Init(void);

// - Receive
void Board_CAN_Receive(BMS_INPUT_T *bms_input);

void can_receive_bms_state(BMS_INPUT_T *bms_input);

void can_receive_vcu_switch(BMS_INPUT_T *bms_input);

void can_receive_current(BMS_INPUT_T *bms_input);

void can_receive_voltage(BMS_INPUT_T *bms_input);

void can_receive_energy(BMS_INPUT_T *bms_input);

// - Transmit
void Board_CAN_Transmit(BMS_INPUT_T  *bms_input,
                        BMS_OUTPUT_T *bms_output);

void can_transmit_contactor_weld(BMS_INPUT_T *bms_input,
                                 uint32_t     msTicks);

void can_transmit_bms_errors(BMS_INPUT_T *bms_input,
                             uint32_t     msTicks);

void can_transmit_pack_status(BMS_INPUT_T *bms_input,
                              uint32_t     msTicks);

void can_transmit_bms_soc(BMS_INPUT_T *bms_input,
                          uint32_t     msTicks);

#endif // ifdef _CANMGR_H
