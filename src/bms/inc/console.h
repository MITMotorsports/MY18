#include <string.h>
#include <stdlib.h>
#include "microrl.h"
#include "state_types.h"
#include "util.h"
#include "console_types.h"
#include "eeprom_config.h"

#ifndef _CONSOLE_H
#define _CONSOLE_H

// [TODO] Add command to reset to factory default

typedef struct {
    char *name;
    command_label_t label;
    uint32_t nargs;
    char *help;
} BMS_COMMAND_T;

static const BMS_COMMAND_T commands[] = {
    {"get",        C_GET,     1, "Get a value. Usage: get [field]"},
    {"set",        C_GET,     2, "Set a value. Usage: set [field] [value]"},
    {"help",       C_HELP,    1, "Get help. Usage: help [command name]"},
    {"config",     C_CONFIG,  0, "Reinitialize BMS, reload EEPROM. Usage: config"},
    {"dis",        C_DIS,     0, "Go into discharge mode. Usage: dis [on|off]"},
    {"measure",    C_MEASURE, 1, "Start measurement printout mode. Usage: ..."},
    {"config_def", C_CONFIG_DEF, 0, "Configure pack config defaults."}
};

static const char * const locstring[] =  {
                            "cell_min_mV",
                            "cell_max_mV",
                            "cell_capacity_cAh",
                            "num_modules",
                            "module_cell_count",
                            "cell_charge_c_rating_cC",
                            "bal_on_thresh_mV",
                            "bal_off_thresh_mV",
                            "pack_cells_p",
                            "cv_min_current_mA",
                            "cv_min_current_ms",
                            "cc_cell_voltage_mV",
                            "cell_discharge_c_rating_cC",
                            "max_cell_temp_param",
                            //can't write to the follwing
                            "state",
                            "cvm",
                            "pack_cell_max_mV",
                            "pack_cell_min_mV",
                            "pack_current_mA",
                            "pack_voltage_mV",
                            "max_cell_temp_dC",
                            "error"
};

static const uint32_t locparam[ARRAY_SIZE(locstring)][3] = { 
                            {1, 0,UINT32_MAX},//"cell_min_mV",
                            {1, 0,UINT32_MAX},//"cell_max_mV",
                            {1, 0,UINT32_MAX},//"cell_capacity_cAh",
                            {1, 0,UINT8_MAX},//"num_modules",
                            {20, 0,UINT8_MAX},//"module_cell_count",
                            {1, 0,UINT16_MAX},//"cell_charge_c_rating_cC",
                            {1, 0,UINT32_MAX},//"bal_on_thresh_mV",
                            {1, 0,UINT32_MAX},//"bal_off_thresh_mV",
                            {1, 0,UINT8_MAX},//"pack_cells_p",
                            {1, 0,UINT32_MAX},//"cv_min_current_mA",
                            {1, 0,UINT32_MAX},//"cv_min_current_ms",
                            {1, 0,UINT32_MAX},//"cc_cell_voltage_mV",
                            {1, 0,UINT32_MAX},//"cell_discharge_c_rating_cC",
                            {1, 0,UINT32_MAX},//"max_cell_temp_dC",
                            //can't write to the follwing
                            {0,0,0},//"state",
                            {0,0,0},//"*cell_voltages_mV",
                            {0,0,0},//"pack_cell_max_mV",
                            {0,0,0},//"pack_cell_min_mV",
                            {0,0,0},//"pack_current_mA",
                            {0,0,0},//"pack_voltage_mV",
                            {0,0,0},//"max_cell_temp_dC"
                            {0,0,0}//"error"
};

typedef void (* const COMMAND_EXECUTE_HANDLER)(const char * const *);

typedef struct {
    bool valid_mode_request;
    bool config_default;
    BMS_SSM_MODE_T mode_request;
    uint32_t balance_mV;
    bool measure_on;
    bool measure_temp;
    bool measure_voltage;
    bool measure_packcurrent;
    bool measure_packvoltage;
} CONSOLE_OUTPUT_T;

typedef struct console_t {
    BMS_INPUT_T     *bms_input;
    BMS_STATE_T     *bms_state;
    CONSOLE_OUTPUT_T *console_output;
} console_t;

// static console_t console;

void console_init(BMS_INPUT_T *bms_input, BMS_STATE_T *bms_state, CONSOLE_OUTPUT_T *console_output);
void executerl(int32_t argc, const char * const * argv);  


#endif
