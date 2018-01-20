#include "chip.h"
#include "state_types.h"
#include "config.h"
#include "ltc6804.h"
#include <stdlib.h>

#ifndef _BOARD_H_
#define _BOARD_H_

#define Hertz2Ticks(freq) SystemCoreClock / freq

#define TIME_PER_THERMISTOR_MS 40

#define UART_BUFFER_SIZE 100


// ltc6804 constants
#define LTC6804_SHIFT_REGISTER_DATA_IN 4
#define LTC6804_SHIFT_REGISTER_CLOCK 3
#define LTC6804_SHIFT_REGISTER_LATCH 2
#define LTC6804_GPIO_COUNT 5

typedef enum {
    LTC6804_INIT_NONE, LTC6804_INIT_CFG, LTC6804_INIT_CVST, LTC6804_INIT_OWT, LTC6804_INIT_DONE
} LTC6804_INIT_STATE_T;


void Board_Chip_Init(void);
void Board_GPIO_Init(void);
void Board_CAN_Init(uint32_t baudrate);
void Board_CAN_ProcessInput(BMS_INPUT_T* bms_input);

bool Board_LTC6804_Init(BMS_PACK_CONFIG_T *pack_config, uint32_t *cell_voltages_mV);
void Board_LTC6804_DeInit(void);
void Board_LTC6804_ProcessInputs(BMS_PACK_STATUS_T *pack_status, BMS_STATE_T* bms_state);
void Board_LTC6804_ProcessOutput(bool *balance_req);

void Board_LTC6804_GetCellVoltages(BMS_PACK_STATUS_T *pack_status);
void Board_LTC6804_GetCellTemperatures(BMS_PACK_STATUS_T * pack_status, uint8_t num_modules);
void Board_PrintThermistorTemperatures(uint8_t module, BMS_PACK_STATUS_T * pack_status);
bool Board_LTC6804_OpenWireTest(void);
void Board_UART_Init(uint32_t baudRateHz);


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

void Board_HandleLtc6804Status(LTC6804_STATUS_T status);

#endif
