#include "discharge.h"

#include "state_types.h"

//this constatnt is from my17 bms, should this be recalculated?
#define MAX_CURRENT 540000

static uint16_t total_num_cells;
static uint32_t min_cell_voltage_mV;
static uint32_t max_pack_current_mA;
static uint16_t max_cell_temp_thres_C;

void Discharge_Init(BMS_STATE_T *state) {
    state->discharge_state = BMS_DISCHARGE_OFF;
}
void Discharge_Config(BMS_PACK_CONFIG_T *pack_config) {
    //because all modules have the same u=number of cells
    total_num_cells = pack_config->num_modules * pack_config->module_cell_count[0];
    min_cell_voltage_mV = pack_config->cell_min_mV;
    max_cell_temp_thres_C = pack_config->max_cell_temp_dC;
    max_pack_current_mA = MAX_CURRENT;
}
