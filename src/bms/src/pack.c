#include "pack.h"

const uint16_t Pack_Config_Total_Cell_Count(BMS_PACK_CONFIG_T *config) {
  uint16_t total = 0;

  uint8_t i = 0;

  for (i = 0; i < config->num_modules; i++) {
    total += config->module_cell_count[i];
  }

  return total;
}

BMS_VOLTAGE_ESTIMATE_T Pack_Estimate_Total_Voltage(BMS_PACK_CONFIG_T *config, BMS_PACK_STATUS_T *state) {
  BMS_VOLTAGE_ESTIMATE_T estimate;

  // TODO: Make static since cell count should not change at runtime.
  uint16_t cellcount = Pack_Config_Total_Cell_Count(config);

  estimate.pack_voltage_mV_upper = cellcount * state->pack_cell_max_mV;
  estimate.pack_voltage_mV_lower = cellcount * state->pack_cell_min_mV;

  return estimate;
}
