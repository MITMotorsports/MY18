#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdlib.h>

#include "chip.h"
#include "state_types.h"
#include "config.h"
#include "ltc6804.h"
#include "console.h"
#include "pins.h"

#include "can.h"
#include "gpio.h"
#include "uart.h"

extern volatile uint32_t msTicks;

#define Hertz2Ticks(freq) SystemCoreClock / freq

#define TIME_PER_THERMISTOR_MS 40

// LTC6804 constants
#define LTC6804_SHIFT_REGISTER_DATA_IN 4
#define LTC6804_SHIFT_REGISTER_CLOCK 3
#define LTC6804_SHIFT_REGISTER_LATCH 2
#define LTC6804_GPIO_COUNT 5

typedef enum {
  LTC6804_INIT_NONE,
  LTC6804_INIT_CFG,
  LTC6804_INIT_CVST,
  LTC6804_INIT_OWT,
  LTC6804_INIT_DONE,
} LTC6804_INIT_STATE_T;


// Functions
void Board_Chip_Init(void);

// -- Delay
void Board_BlockingDelay(uint32_t delayms);


// -- Get state machine mode request from console
void Board_GetModeRequest(const CONSOLE_OUTPUT_T *console_output,
                          BMS_INPUT_T            *bms_input);


// LTC6804 Board Functions
bool Board_LTC6804_Init(BMS_PACK_CONFIG_T *pack_config,
                        uint32_t          *cell_voltages_mV);

void Board_LTC6804_DeInit(void);

// Gets Voltages, Temperatures, and runs OpenWireTest
void Board_LTC6804_ProcessInputs(BMS_PACK_STATUS_T *pack_status,
                                 BMS_STATE_T       *bms_state);

// Updates Balancing Parameters
void Board_LTC6804_UpdateBalanceStates(bool *balance_req);

void Board_LTC6804_GetCellVoltages(BMS_PACK_STATUS_T *pack_status);

void Board_LTC6804_GetCellTemperatures(BMS_PACK_STATUS_T *pack_status,
                                       uint8_t            num_modules);

// Prints temperatures of specific module
void Board_PrintThermistorTemperatures(uint8_t            module,
                                       BMS_PACK_STATUS_T *pack_status);

// Does CVST Check
bool Board_LTC6804_CVST(void);

// Does OpenWireTest Check
bool Board_LTC6804_OpenWireTest(void);

// Handles Status of LTC6804 drier
void Board_HandleLtc6804Status(LTC6804_STATUS_T status);

#endif // ifndef _BOARD_H_
