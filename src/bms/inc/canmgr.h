#define _CANMGR_H
#ifdef _CANMGR_H

#include <stdint.h>
#include "CANlib.h"

#include "board.h"
#include "pack.h"
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

void can_receive_bms_request(BMS_INPUT_T *bms_input);

void can_receive_current(BMS_INPUT_T *bms_input);

void can_receive_voltage(BMS_INPUT_T *bms_input);

void can_receive_energy(BMS_INPUT_T *bms_input);

// - Transmit
void Board_CAN_Transmit(BMS_INPUT_T  *bms_input,
                        BMS_OUTPUT_T *bms_output);

void can_transmit_bms_heartbeat(BMS_INPUT_T *bms_input);

void can_transmit_cell_temperatures(BMS_INPUT_T *bms_input);

void can_transmit_cell_voltages(BMS_INPUT_T *bms_input);

#endif // ifdef _CANMGR_H
