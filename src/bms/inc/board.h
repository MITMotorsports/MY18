#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"

#ifndef _BOARD_H_
#define _BOARD_H_

#define Hertz2Ticks(freq) SystemCoreClock / freq
void Board_Chip_Init(void);
void Board_GPIO_Init(void);
void Board_CAN_Init(uint32_t baudrate);
void Board_CAN_ProcessInput(void/*BMS_INPUT,OUTPUT*/);
void Board_LTC6804_ProcessInputs(void /*pack status*/);
void Board_LTC6804_GetCellVoltages(void/*pack_status*/);
void Board_LTC6804_GetCellTemperatures(void /*pack_status, bms_state->pack_config->num_modules*/);
void Board_LTC6804_OpenWireTest(void);
#endif