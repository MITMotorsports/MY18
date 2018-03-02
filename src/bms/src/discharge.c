#include "discharge.h"

#include "state_types.h"

// this constatnt is from my17 bms, should this be recalculated?
#define MAX_CURRENT 540000

static uint16_t total_num_cells;
static uint32_t min_cell_voltage_mV;
static uint32_t max_pack_current_mA;
static uint16_t max_cell_temp_thres_C;

void Discharge_Init(BMS_STATE_T *state) {
  state->discharge_state = BMS_DISCHARGE_OFF;
}

void Discharge_Config(BMS_PACK_CONFIG_T *pack_config) {
  // because all modules have the same number of cells..?
  total_num_cells = pack_config->num_modules *
                    pack_config->module_cell_count[0];
  min_cell_voltage_mV   = pack_config->cell_min_mV;
  max_cell_temp_thres_C = pack_config->max_cell_temp_dC;
  max_pack_current_mA   = MAX_CURRENT;
}

void Discharge_Step(BMS_INPUT_T *input, BMS_STATE_T *state,
                    BMS_OUTPUT_T *output) {
handler:

  switch (input->mode_request) {
  case BMS_SSM_MODE_DISCHARGE:

    if (state->discharge_state == BMS_DISCHARGE_OFF) {
      state->discharge_state = BMS_DISCHARGE_INIT;
    } break;

  default:

    if (state->discharge_state != BMS_DISCHARGE_OFF) {
      state->discharge_state = BMS_DISCHARGE_DONE;
    }
    break;
  }

  switch (state->discharge_state) {
  case BMS_DISCHARGE_OFF:
    output->close_contactors = false;
    break;

  case BMS_DISCHARGE_INIT:
    output->close_contactors = true;

    // checking if the fault pin has correctly been set to high
    if (input->contactors_closed == output->close_contactors) {
      state->discharge_state = BMS_DISCHARGE_RUN;
      goto handler;
    }
    break;

  case BMS_DISCHARGE_RUN:
    output->close_contactors = true;

    // recalculate max current?

    /*            if(input->pack_status->pack_current_mA > max_pack_current_mA)
       {
                     Error_Assert(ERROR_OVER_CURRENT, input->msTicks);
                    Error_Pass(ERROR_OVER_CURRENT);
                } else {
                    Error_Pass(ERROR_OVER_CURRENT);
                }
     */

    // if the fault pin is low, go back to init and change it to high
    if (!input->contactors_closed) {
      state->discharge_state = BMS_DISCHARGE_INIT;
    }

    // missig fan logic and vcu heartbeat logic here
    break;

  case BMS_DISCHARGE_DONE:
    output->close_contactors = false;

    if (!input->contactors_closed) {
      state->discharge_state = BMS_DISCHARGE_OFF;
    }
    break;
  }
}
