#include <string.h>
#include "eeprom_config.h"
#include "chip.h"
#include "ltc6804.h"
#include "board.h"
#include "pack.h"
#include "state_types.h"
#include "config.h"
#include "console.h"
#include "ssm.h"
#include "error_handler.h"
#include "soc.h"

#define EEPROM_CS_PIN 2, 9
#define PRE_ERROR_CHECK_TIMEOUT 500

static char str[10];

static BMS_PACK_STATUS_T pack_status;
static BMS_INPUT_T  bms_input;
static BMS_OUTPUT_T bms_output;
static BMS_STATE_T  bms_state;

static BMS_PACK_CONFIG_T pack_config;
static uint32_t cell_voltages[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];
static int16_t  cell_temperatures[MAX_NUM_MODULES * MAX_THERMISTORS_PER_MODULE];
static uint8_t  module_cell_count[MAX_NUM_MODULES];

// memory allocation for BMS_OUTPUT balancing
static bool balance_reqs[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];
static bool balance_waitingoff[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];
static uint32_t balance_timeon[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];

// memory for console
static microrl_t rl;
static CONSOLE_OUTPUT_T console_output;
