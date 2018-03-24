#include "eeprom_config.h"
#include "error_handler.h"
#include "board.h"

#include <string.h>


#define DATA_BLOCK_SIZE sizeof(BMS_PACK_CONFIG_T) + ERROR_BYTESIZE + \
  CHECKSUM_BYTESIZE + VERSION_BYTESIZE + MAX_NUM_MODULES
#define CC_PAGE_SZ 64
static uint8_t eeprom_data_buf[DATA_BLOCK_SIZE];
static BMS_PACK_CONFIG_T eeprom_packconf_buf;
static uint8_t mcc[MAX_NUM_MODULES];
static uint8_t eeprom_data_addr_pckcfg[3]; // LC1024 eeprom address length is 3
                                           // bytes
static uint8_t eeprom_data_addr_cc[3];
static uint8_t saved_bms_error;


static uint8_t Calculate_Checksum(BMS_PACK_CONFIG_T *pack_config);
static void    Zero_EEPROM_DataBuffer(void);
static void    Write_PackConfig_EEPROM(void);


static void    Zero_EEPROM_DataBuffer(void) {
  uint8_t i;

  for (i = 0; i < DATA_BLOCK_SIZE; i++) {
    eeprom_data_buf[i] = 0;
  }
}

void EEPROM_Init(LPC_SSP_T *pSSP, uint32_t baud, uint8_t cs_gpio,
                 uint8_t cs_pin) {
  LC1024_Init(pSSP, baud, cs_gpio, cs_pin);

  eeprom_data_addr_pckcfg[0] = EEPROM_DATA_START_PCKCFG >> 16;
  eeprom_data_addr_pckcfg[1] = (EEPROM_DATA_START_PCKCFG & 0xFF00) >> 8;
  eeprom_data_addr_pckcfg[2] = (EEPROM_DATA_START_PCKCFG & 0xFF);

  eeprom_data_addr_cc[0] = EEPROM_DATA_START_CC >> 16;
  eeprom_data_addr_cc[1] = (EEPROM_DATA_START_CC & 0xFF00) >> 8;
  eeprom_data_addr_cc[2] = (EEPROM_DATA_START_CC & 0xFF);

  // Run_EEPROM_Test();

  Zero_EEPROM_DataBuffer();
  eeprom_packconf_buf.module_cell_count = mcc;
  saved_bms_error                       = 255;

  Board_Println_BLOCKING("Finished EEPROM init...");
  Board_BlockingDelay(200);
}

void EEPROM_WriteCCPage(uint32_t *cc) {
  Board_Println("Writing CC Page to EEPROM...");
  memcpy(eeprom_data_buf, cc, CC_PAGE_SZ);
  LC1024_WriteEnable();
  LC1024_WriteEnable();
  LC1024_WriteMem(eeprom_data_addr_cc, eeprom_data_buf, CC_PAGE_SZ);
  // Board_BlockingDelay(150);
  Board_Println("Done writing CC Page to EEPROM...");
}

void EEPROM_LoadCCPage(uint32_t *cc) {
  Board_Println_BLOCKING("Loading CC Page from EEPROM...");
  LC1024_WriteEnable();
  LC1024_WriteEnable();
  LC1024_ReadMem(eeprom_data_addr_cc, eeprom_data_buf, CC_PAGE_SZ);
  Board_BlockingDelay(150);
  memcpy(cc, eeprom_data_buf, CC_PAGE_SZ);
  Board_Println_BLOCKING("Done loading CC Page from EEPROM...");
}

// idx should be from 0-63 inclusive
void EEPROM_WriteCCPage_Num(uint8_t idx, uint32_t val) {
  Board_Println_BLOCKING("Writing CC Num to EEPROM...");

  LC1024_WriteEnable();
  LC1024_WriteEnable();
  LC1024_ReadMem(eeprom_data_addr_cc, eeprom_data_buf, CC_PAGE_SZ);
  Board_BlockingDelay(150);

  eeprom_data_buf[idx << 2]       = val >> 24;
  eeprom_data_buf[(idx << 2) + 1] = (val & 0x00FF0000) >> 16;
  eeprom_data_buf[(idx << 2) + 2] = (val & 0x0000FF00) >> 8;
  eeprom_data_buf[(idx << 2) + 3] = (val & 0x000000FF);

  LC1024_WriteEnable();
  LC1024_WriteEnable();
  LC1024_WriteMem(eeprom_data_addr_cc, eeprom_data_buf, CC_PAGE_SZ);
  Board_BlockingDelay(150);

  Board_Println_BLOCKING("Done writing CC Num to EEPROM...");
}

// idx should be from 0-63 inclusive
uint32_t EEPROM_LoadCCPage_Num(uint8_t idx) {
  Board_Println_BLOCKING("Loading CC Num from EEPROM...");
  LC1024_WriteEnable();
  LC1024_WriteEnable();
  LC1024_ReadMem(eeprom_data_addr_cc, eeprom_data_buf, CC_PAGE_SZ);
  Board_BlockingDelay(150);
  Board_Println_BLOCKING("Done loading CC Num from EEPROM...");
  return (eeprom_data_buf[idx << 2] << 24)
         + (eeprom_data_buf[(idx << 2) + 1] << 16)
         + (eeprom_data_buf[(idx << 2) + 2] << 8)
         + eeprom_data_buf[(idx << 2) + 3];
}

void Print_EEPROM_Error(void) {
  Board_Print("Error saved in EEPROM: ");

  if (saved_bms_error < ERROR_NUM_ERRORS) {
    Board_Println(ERROR_NAMES[saved_bms_error]);
  } else {
    Board_Println("No Error!");
  }
}

uint8_t Get_EEPROM_Error(void) {
  return saved_bms_error;
}

void Set_EEPROM_Error(uint8_t error) {
  Board_Print("Error to be saved in EEPROM next cycle: ");

  if (error < ERROR_NUM_ERRORS) {
    Board_Println(ERROR_NAMES[error]);
  } else {
    Board_Println("No Error!");
  }
  saved_bms_error = error;
}

static void Write_PackConfig_EEPROM(void) {
  // offset in the below line: we do not copy the module cell count ptr (1 byte)
  memcpy(eeprom_data_buf,
         &eeprom_packconf_buf,
         sizeof(BMS_PACK_CONFIG_T) - sizeof(void *));
  memcpy(&eeprom_data_buf[sizeof(BMS_PACK_CONFIG_T)],
         mcc,
         MAX_NUM_MODULES);
  eeprom_data_buf[DATA_BLOCK_SIZE - CHECKSUM_BYTESIZE - VERSION_BYTESIZE -
                  ERROR_BYTESIZE] = STORAGE_VERSION;
  eeprom_data_buf[DATA_BLOCK_SIZE - CHECKSUM_BYTESIZE -
                  ERROR_BYTESIZE] = Calculate_Checksum(&eeprom_packconf_buf);
  eeprom_data_buf[DATA_BLOCK_SIZE -
                  ERROR_BYTESIZE] = saved_bms_error;


  LC1024_WriteEnable();
  LC1024_WriteEnable();

  LC1024_WriteMem(eeprom_data_addr_pckcfg, eeprom_data_buf, DATA_BLOCK_SIZE);
  Board_BlockingDelay(200);
  Board_Println_BLOCKING("Finished writing pack config to EEPROM.");
}

void Write_EEPROM_Error(void) {
  Write_PackConfig_EEPROM();
}

// SHOULD ONLY BE CALLED IN STANDBY MODE, so only need to r


static uint8_t Calculate_Checksum(BMS_PACK_CONFIG_T *pack_config) {
  uint8_t checksum = 0;
  char   *data     = (char *)pack_config;
  uint8_t i;

  for (i = 0; i < sizeof(BMS_PACK_CONFIG_T); i++) {
    checksum += *data++;
    checksum  = checksum % 256;
  }
  return checksum;
}
