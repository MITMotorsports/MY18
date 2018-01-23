#include "eeprom_config.h"
#include "error_handler.h"
#include "board.h"

#include <string.h>


#define DATA_BLOCK_SIZE sizeof(BMS_PACK_CONFIG_T) + ERROR_BYTESIZE + CHECKSUM_BYTESIZE + VERSION_BYTESIZE + MAX_NUM_MODULES
#define CC_PAGE_SZ 64
static uint8_t eeprom_data_buf[DATA_BLOCK_SIZE];
static BMS_PACK_CONFIG_T eeprom_packconf_buf;
static uint8_t mcc[MAX_NUM_MODULES];
static uint8_t eeprom_data_addr_pckcfg[3]; // LC1024 eeprom address length is 3 bytes
static uint8_t eeprom_data_addr_cc[3];
static uint8_t saved_bms_error;


static bool Validate_PackConfig(BMS_PACK_CONFIG_T *pack_config, uint16_t version, uint8_t checksum);
static uint8_t Calculate_Checksum(BMS_PACK_CONFIG_T *pack_config);
static void Load_PackConfig_Defaults(BMS_PACK_CONFIG_T *pack_config);
static void Zero_EEPROM_DataBuffer(void);
static void Write_PackConfig_EEPROM(void);
// static void Print_EEPROM_DataBuffer(void);
// static void Run_EEPROM_Test(void);

static void Zero_EEPROM_DataBuffer(void) {
    uint8_t i;
    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        eeprom_data_buf[i] = 0;
    }
}

// static void Print_EEPROM_DataBuffer(void) {
//     Board_Print_BLOCKING("0x");
//     uint8_t i;
//     for(i = 0; i < DATA_BLOCK_SIZE; i++) {
//         itoa(eeprom_data_buf[i], str, 16);
//         if(eeprom_data_buf[i] < 16) {
//             Board_Print_BLOCKING("0");
//          Board_Print_BLOCKING(str);
//         } else {
//          Board_Print_BLOCKING(str);
//      }
//         Board_Print_BLOCKING(" ");
//     }
//     Board_Println_BLOCKING("");
// }

// static void Run_EEPROM_Test(void) {
//     Zero_EEPROM_DataBuffer();
//     LC1024_WriteEnable();
//     LC1024_WriteEnable();
//     LC1024_ReadStatusReg(eeprom_data_buf);
//  Board_Println_BLOCKING("Running EEPROM test...");
//
//     eeprom_data_buf[0] = 0x0E;
//     eeprom_data_buf[1] = 0x0E;
//
//     LC1024_WriteMem(eeprom_data_addr, eeprom_data_buf, 2);
//  Board_BlockingDelay(200);
//
//     LC1024_ReadMem(eeprom_data_addr, eeprom_data_buf, 2);
//  Board_BlockingDelay(200);
//
//     if(eeprom_data_buf[0] == 0xE) {
//         Board_Println_BLOCKING("EEPROM passed write test!");
//     } else {
//         Board_Println_BLOCKING("EEPROM failed write test!");
//     }
// }

void EEPROM_Init(LPC_SSP_T *pSSP, uint32_t baud, uint8_t cs_gpio, uint8_t cs_pin){
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
    saved_bms_error = 255;

    Board_Println_BLOCKING("Finished EEPROM init...");
    Board_BlockingDelay(200);
}

void EEPROM_WriteCCPage(uint32_t *cc) {
    Board_Println_BLOCKING("Writing CC Page to EEPROM...");
    memcpy(eeprom_data_buf, cc, CC_PAGE_SZ);
    LC1024_WriteEnable();
    LC1024_WriteEnable();
    LC1024_WriteMem(eeprom_data_addr_cc, eeprom_data_buf, CC_PAGE_SZ);
    Board_BlockingDelay(150);
    Board_Println_BLOCKING("Done writing CC Page to EEPROM...");
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

    eeprom_data_buf[idx<<2] = val >> 24;
    eeprom_data_buf[(idx<<2)+1] = (val & 0x00FF0000) >> 16;
    eeprom_data_buf[(idx<<2)+2] = (val & 0x0000FF00) >> 8;
    eeprom_data_buf[(idx<<2)+3] = (val & 0x000000FF);

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
    return ((eeprom_data_buf[idx<<2] << 24)
            + (eeprom_data_buf[(idx<<2)+1] << 16)
            + (eeprom_data_buf[(idx<<2)+2] << 8)
            + eeprom_data_buf[(idx<<2)+3]);
}

// entry from Process_Output(..) in main.c, executed during start
bool EEPROM_LoadPackConfig(BMS_PACK_CONFIG_T *pack_config) {

    Board_Println_BLOCKING("Loading PackConfig from EEPROM...");
    LC1024_WriteEnable();
    LC1024_WriteEnable();
    LC1024_ReadMem(eeprom_data_addr_pckcfg, eeprom_data_buf, DATA_BLOCK_SIZE);
    Board_BlockingDelay(200);

    // offset in the below line: we do not copy module cell count ptr (1 byte)
    // loading into the eeprom driver packconfig buffer
    memcpy(&eeprom_packconf_buf, eeprom_data_buf, sizeof(BMS_PACK_CONFIG_T)-sizeof(void*));
    memcpy(mcc, &eeprom_data_buf[sizeof(BMS_PACK_CONFIG_T)], MAX_NUM_MODULES);
    uint16_t version = eeprom_data_buf[DATA_BLOCK_SIZE - CHECKSUM_BYTESIZE - VERSION_BYTESIZE - ERROR_BYTESIZE];
    uint8_t checksum = eeprom_data_buf[DATA_BLOCK_SIZE - CHECKSUM_BYTESIZE - ERROR_BYTESIZE];
    saved_bms_error = eeprom_data_buf[DATA_BLOCK_SIZE - ERROR_BYTESIZE];


//commented out for testing purposes, to directly use pre-configured defaults

/*    if (Validate_PackConfig(&eeprom_packconf_buf, version, checksum)) {
        Board_Println_BLOCKING("Passed validation, using load from EEPROM...");
        // loading from eeprom driver packconfig buffer
        uint8_t * their_mccp = pack_config->module_cell_count;
        memcpy(pack_config, &eeprom_packconf_buf, sizeof(BMS_PACK_CONFIG_T)-sizeof(void*));
        pack_config->module_cell_count = their_mccp;
        memcpy(pack_config->module_cell_count, mcc, MAX_NUM_MODULES);
        return true;
    } else {
        Board_Println_BLOCKING("Using pre-configured defaults...");
        Load_PackConfig_Defaults(pack_config);
        Load_PackConfig_Defaults(&eeprom_packconf_buf);
        Board_Println_BLOCKING("Finished loading pre-configured defaults...");
        Write_PackConfig_EEPROM();
        Board_Println_BLOCKING("Wrote pre-configured defaults to EEPROM.");
        return true;
    }*/
        Board_Println_BLOCKING("Using pre-configured defaults...");
        Load_PackConfig_Defaults(pack_config);
        //Load_PackConfig_Defaults(&eeprom_packconf_buf);
        Board_Println_BLOCKING("Finished loading pre-configured defaults...");
        //Write_PackConfig_EEPROM();
        Board_Println_BLOCKING("Wrote pre-configured defaults to EEPROM.");
        return true;

}

void Print_EEPROM_Error(void) {
    Board_Print("Error saved in EEPROM: ");
    if(saved_bms_error < ERROR_NUM_ERRORS) {
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
    if(error < ERROR_NUM_ERRORS) {
        Board_Println(ERROR_NAMES[error]);
    } else {
        Board_Println("No Error!");
    }
    saved_bms_error = error;
}

void Write_EEPROM_Error(void) {
    Write_PackConfig_EEPROM();
}

void Write_EEPROM_PackConfig_Defaults(void) {
    Board_Println_BLOCKING("Using pre-configured defaults...");
    LC1024_WriteEnable();
    LC1024_WriteEnable();
    Load_PackConfig_Defaults(&eeprom_packconf_buf);
    Board_Println_BLOCKING("Finished loading pre-configured defaults...");
    Write_PackConfig_EEPROM();
    Board_Println_BLOCKING("Wrote pre-configured defaults to EEPROM.");
}

static void Load_PackConfig_Defaults(BMS_PACK_CONFIG_T *pack_config) {
    pack_config->cell_min_mV = CELL_MIN_mV;
    pack_config->cell_max_mV = CELL_MAX_mV;
    pack_config->cell_capacity_cAh = CELL_CAPACITY_cAh;
    pack_config->num_modules = NUM_MODULES;
    pack_config->cell_charge_c_rating_cC = CELL_CHARGE_C_RATING_cC;
    pack_config->bal_on_thresh_mV = BALANCE_ON_THRESHOLD_mV;
    pack_config->bal_off_thresh_mV = BALANCE_OFF_THRESHOLD_mV;
    pack_config->pack_cells_p = PACK_CELLS_PARALLEL;
    pack_config->cv_min_current_mA = CV_MIN_CURRENT_mA;
    pack_config->cv_min_current_ms = CV_MIN_CURRENT_ms;
    pack_config->cc_cell_voltage_mV = CC_CELL_VOLTAGE_mV;
    pack_config->cell_discharge_c_rating_cC = CELL_DISCHARGE_C_RATING_cC;
    pack_config->max_cell_temp_dC = MAX_CELL_TEMP_dC;

    // FSAE specific pack configurations
#ifdef FSAE_DRIVERS
    pack_config->fan_on_threshold_dC = FAN_ON_THRESHOLD_dC;
    pack_config->min_cell_temp_dC = MIN_CELL_TEMP_dC;
#endif //FSAE_DRIVERS

    uint8_t i;
    for(i = 0; i < MAX_NUM_MODULES; i++) {
        pack_config->module_cell_count[i] = MODULE_CELL_COUNT;
    }
}

// SHOULD ONLY BE CALLED IN STANDBY MODE, so only need to run checks
uint8_t EEPROM_ChangeConfig(rw_loc_label_t rw_loc, uint32_t val) {
    switch (rw_loc) {
        case RWL_cell_min_mV:
            eeprom_packconf_buf.cell_min_mV = val;
            break;
        case RWL_cell_max_mV:
            eeprom_packconf_buf.cell_max_mV = val;
            break;
        case RWL_cell_capacity_cAh:
            eeprom_packconf_buf.cell_capacity_cAh = val;
            break;
        case RWL_num_modules:
            eeprom_packconf_buf.num_modules = val;
            break;
        case RWL_module_cell_count:
            // TODO
            // module_cell_count[0...15] =
            return 1;
        case RWL_cell_charge_c_rating_cC:
            eeprom_packconf_buf.cell_charge_c_rating_cC = val;
            break;
        case RWL_bal_on_thresh_mV:
            eeprom_packconf_buf.bal_on_thresh_mV = val;
            break;
        case RWL_bal_off_thresh_mV:
            eeprom_packconf_buf.bal_off_thresh_mV = val;
            break;
        case RWL_pack_cells_p:
            eeprom_packconf_buf.pack_cells_p = val;
            break;
        case RWL_cv_min_current_mA:
            eeprom_packconf_buf.cv_min_current_mA = val;
            break;
        case RWL_cv_min_current_ms:
            eeprom_packconf_buf.cv_min_current_ms = val;
            break;
        case RWL_cc_cell_voltage_mV:
            eeprom_packconf_buf.cc_cell_voltage_mV = val;
            break;
        case RWL_cell_discharge_c_rating_cC:
            eeprom_packconf_buf.cell_discharge_c_rating_cC = val;
            break;
        case RWL_max_cell_temp_dC:
            eeprom_packconf_buf.max_cell_temp_dC = val;
            break;
        case RWL_LENGTH:
            break;
    }

    Write_PackConfig_EEPROM();
    return 0;
}

static void Write_PackConfig_EEPROM(void) {

    // offset in the below line: we do not copy the module cell count ptr (1 byte)
    memcpy(eeprom_data_buf, &eeprom_packconf_buf, sizeof(BMS_PACK_CONFIG_T)-sizeof(void*));
    memcpy(&eeprom_data_buf[sizeof(BMS_PACK_CONFIG_T)], mcc, MAX_NUM_MODULES);
    eeprom_data_buf[DATA_BLOCK_SIZE - CHECKSUM_BYTESIZE - VERSION_BYTESIZE - ERROR_BYTESIZE] = STORAGE_VERSION;
    eeprom_data_buf[DATA_BLOCK_SIZE - CHECKSUM_BYTESIZE - ERROR_BYTESIZE] = Calculate_Checksum(&eeprom_packconf_buf);
    eeprom_data_buf[DATA_BLOCK_SIZE - ERROR_BYTESIZE] = saved_bms_error;


    LC1024_WriteEnable();
    LC1024_WriteEnable();

    LC1024_WriteMem(eeprom_data_addr_pckcfg, eeprom_data_buf, DATA_BLOCK_SIZE);
    Board_BlockingDelay(200);
    Board_Println_BLOCKING("Finished writing pack config to EEPROM.");
}


static uint8_t Calculate_Checksum(BMS_PACK_CONFIG_T *pack_config) {
    uint8_t checksum = 0;
    char *data = (char *) pack_config;
    uint8_t i;
    for (i=0; i < sizeof(BMS_PACK_CONFIG_T); i++) {
        checksum += *data++;
        checksum = checksum % 256;
    }
    return checksum;
}

static bool Validate_PackConfig(BMS_PACK_CONFIG_T *pack_config, uint16_t version, uint8_t checksum) {
    uint8_t calc_checksum = Calculate_Checksum(pack_config);
    if(version != STORAGE_VERSION) {
        Board_Println_BLOCKING("Storage version check failed!");
        return false;
    } else if(calc_checksum != checksum) {
        Board_Println_BLOCKING("Checksum check failed!");
        return false;
    }
    bool check = pack_config->cell_discharge_c_rating_cC < 500;
    check &= pack_config->cell_charge_c_rating_cC < 500;
    check &= pack_config->cell_min_mV < 10000;
    check &= pack_config->cell_max_mV > 1000;
    check &= pack_config->num_modules < 30;
    check &= pack_config->bal_on_thresh_mV < 1000;
    check &= pack_config->bal_off_thresh_mV < 1000;
    if(!check) {
        Board_Println_BLOCKING("Values in PACK_CONFIG are nonsensical! Pack validation failed!");
        return false;
    }
    return true;
}
