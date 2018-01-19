#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "state_types.h"
#ifndef _BOARD_H_
#define _BOARD_H_

#define Hertz2Ticks(freq) SystemCoreClock / freq
void Board_Chip_Init(void);
void Board_GPIO_Init(void);
void Board_CAN_Init(uint32_t baudrate);
void Board_CAN_ProcessInput(BMS_INPUT_T* bms_input);
void Board_LTC6804_ProcessInputs(void /*pack status*/);
void Board_LTC6804_GetCellVoltages(void/*pack_status*/);
void Board_LTC6804_GetCellTemperatures(void /*pack_status, bms_state->pack_config->num_modules*/);
void Board_LTC6804_OpenWireTest(void);



void Board_Headroom_Toggle(void);

void Board_BlockingDelay(uint32_t dlyTicks);

bool Board_Switch_Read(uint8_t gpio_port, uint8_t pin);

uint32_t Board_Print(const char *str);

/**
 * @details Non-blocking printing for user interface. Appends a newline
 *
 * @param str string to print
 */
uint32_t Board_Println(const char *str);

uint32_t Board_PrintNum(uint32_t num, uint8_t base);
/**
 * @details Non-blocking printing for user interface. Appends a newline
 *
 * @param str string to print
 */
uint32_t Board_Write(const char *str, uint32_t count);

uint32_t Board_Read(char *charBuffer, uint32_t length);

uint32_t Board_Print_BLOCKING(const char *str);

uint32_t Board_Println_BLOCKING(const char *str);
#endif
