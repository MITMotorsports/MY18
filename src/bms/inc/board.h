#include "chip.h"
#include "state_types.h"
#include "config.h"
#include "ltc6804.h"
#include "console.h"
#include "pins.h"
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

void Board_ADC_Init(void);

void Board_LED_On(uint8_t led_gpio, uint8_t led_pin);

void Board_LED_Off(uint8_t led_gpio, uint8_t led_pin);

void Board_LED_Toggle(uint8_t led_gpio, uint8_t led_pin);

void Board_UART_Init(uint32_t baudRateHz);

void Board_CAN_Init(uint32_t baudrate);

void Board_CAN_Receive(BMS_INPUT_T *bms_input);

void can_receive_bms_state(BMS_INPUT_T *bms_input);

void can_proces_vcu_switch(BMS_INPUT_T *bms_input);

void can_receive_current(BMS_INPUT_T *bms_input);

void can_receive_voltage(BMS_INPUT_T *bms_input);

void can_receive_energy(BMS_INPUT_T *bms_input);

void Board_CAN_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output);

void can_transmit_contactor_weld(BMS_INPUT_T *bms_input, uint32_t msTicks);

void can_transmit_bms_errors(BMS_INPUT_T *bms_input, uint32_t msTicks);

void can_transmit_pack_status(BMS_INPUT_T *bms_input, uint32_t msTicks);

void can_transmit_bms_soc(BMS_INPUT_T *bms_input, uint32_t msTicks);

//Get state machine mode request from console
void Board_GetModeRequest(const CONSOLE_OUTPUT_T *console_output, BMS_INPUT_T* bms_input);


//LTC6804 Board Functions
bool Board_LTC6804_Init(BMS_PACK_CONFIG_T *pack_config, uint32_t *cell_voltages_mV);

void Board_LTC6804_DeInit(void);

//Gets Voltages, Temperatures, and runs OpenWireTest
void Board_LTC6804_ProcessInputs(BMS_PACK_STATUS_T *pack_status, BMS_STATE_T* bms_state);

//Updates Balancing Parameters
void Board_LTC6804_UpdateBalanceStates(bool *balance_req);

void Board_LTC6804_GetCellVoltages(BMS_PACK_STATUS_T *pack_status);

void Board_LTC6804_GetCellTemperatures(BMS_PACK_STATUS_T * pack_status, uint8_t num_modules);

//prints temperatures of specific module
void Board_PrintThermistorTemperatures(uint8_t module, BMS_PACK_STATUS_T * pack_status);

//Does CVST
bool Board_LTC6804_CVST(void);

//Does OpenWireTEst
bool Board_LTC6804_OpenWireTest(void);

//Handles Status of LTC6804 drier
void Board_HandleLtc6804Status(LTC6804_STATUS_T status);

//Set Pin States
void Board_Contactors_Set(bool close_contactors);

//Gets pin states
bool Board_Contactors_Closed(void);

//Digital
bool Board_Contactor_One_Welded(void);

bool Board_Contactor_Two_Welded(void);

//Analog
// void Board_Contactor_Weld_One(int16_t* adc_data);
// void Board_Contactor_Weld_Two(int16_t* adc_data);

//Non-Blocking
uint32_t Board_Print(const char *str);

uint32_t Board_Println(const char *str);

uint32_t Board_PrintNum(uint32_t num, uint8_t base);

//Read/Write from UART buffer
uint32_t Board_Write(const char *str, uint32_t count);

uint32_t Board_Read(char *charBuffer, uint32_t length);

//Blocking
uint32_t Board_Print_BLOCKING(const char *str);

uint32_t Board_Println_BLOCKING(const char *str);

void Board_BlockingDelay(uint32_t dlyTicks);

#endif
