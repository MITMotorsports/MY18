#ifndef _FSAE_CAN_H
#define _FSAE_CAN_H

#include <stdint.h>
#include "state_types.h"
#include "board.h"
#include "can_raw.h"

void Can_Receive(BMS_INPUT_T *bms_input);

void Can_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output);

void handle_can_error(Can_ErrorID_T err);

#endif
