#ifndef _EEPROM_CONFIG_H_
#define _EEPROM_CONFIG_H_

#include "chip.h"
#include "state_types.h"
#include "pack.h"
#include "console_types.h"
#include "state_types.h"
#include "lc1024.h"
#include "config.h"
#include "board.h"
#include "pack.h"

#define EEPROM_DATA_START_PCKCFG 0x000000
#define EEPROM_DATA_START_CC 0x000100
#define STORAGE_VERSION 0x05
#define CHECKSUM_BYTESIZE 1
#define VERSION_BYTESIZE 1
#define ERROR_BYTESIZE 1


// FSAE specific macros
#define MIN_CELL_TEMP_dC -30
#define FAN_ON_THRESHOLD_dC 450

void EEPROM_Init(LPC_SSP_T *pSSP,
                 uint32_t   baud,
                 uint8_t    cs_gpio,
                 uint8_t    cs_pin);

uint8_t  EEPROM_ChangeConfig(rw_loc_label_t rw_loc,
                             uint32_t       val);

bool     EEPROM_LoadPackConfig(BMS_PACK_CONFIG_T *pack_config);

void     Write_EEPROM_PackConfig_Defaults(void);

void     Print_EEPROM_Error(void);

void     Write_EEPROM_Error(void);

void     Set_EEPROM_Error(uint8_t error);

uint8_t  Get_EEPROM_Error(void);

// Write and Load Values from EEPROM
uint32_t EEPROM_LoadCCPage_Num(uint8_t idx);

void     EEPROM_WriteCCPage_Num(uint8_t  idx,
                                uint32_t val);

void     EEPROM_LoadCCPage(uint32_t *cc);

void     EEPROM_WriteCCPage(uint32_t *cc);
#endif // ifndef _EEPROM_CONFIG_H_
