#define _CANMGR_H
#ifdef _CANMGR_H

#include <stdint.h>
#include "CANlib.h"

#include "board.h"
#include "pack.h"
#include "state_types.h"
#include "error_handler.h"
#include "config.h"

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (msTicks - last_sent < period) return;                 \
  last_sent = msTicks;

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

void handle_can_error(Can_ErrorID_T error);

#endif // ifdef _CANMGR_H
