// ltc-battery-management-system
#include "cell_temperatures.h"
#include <stdbool.h>
#include "board.h"
#include "config.h"
#include "state_types.h"

// lpc11cx4-library
#include "lpc_types.h"

bool CellTemperatures_IgnoreCell(uint16_t cell_id) {
  switch (cell_id) {
  case 96:
    return true;
  }

  return false;
}

void CellTemperatures_UpdateCellTemperaturesArray(uint32_t *gpioVoltages,
                                                  uint8_t currentThermistor,
                                                  BMS_PACK_CONFIG_T *pack_config,
                                                  BMS_PACK_STATUS_T *pack_status,
                                                  bool with_offsets,
                                                  uint8_t num_modules) {
  int16_t thermistorTemperatures[MAX_NUM_MODULES];

  getThermistorTemperatures(gpioVoltages, thermistorTemperatures, num_modules);

  for (uint8_t module = 0; module < num_modules; module++) {
    size_t idx = module * MAX_THERMISTORS_PER_MODULE + currentThermistor;
    pack_status->cell_temperatures_dC[idx] =
        thermistorTemperatures[module] +
        (with_offsets ? pack_config->cell_temperature_offsets_dC[idx] : 0);
  }
}

void CellTemperatures_UpdateMaxMinAvgCellTemperatures(BMS_PACK_STATUS_T *pack_status,
                                                      uint8_t num_modules) {
  int16_t maxCellTemperature          = INT16_MIN;
  int16_t minCellTemperature          = INT16_MAX;
  int32_t cellTemperaturesSum         = 0;
  uint16_t maxCellTemperaturePosition = 0;
  uint16_t minCellTemperaturePosition = 0;

  unsigned ignorecnt = 0;

  for (uint8_t module = 0; module < num_modules; module++) {
    // 255 * 24 < UINT16_MAX so this is safe
    uint16_t start = module * MAX_THERMISTORS_PER_MODULE;

    for (uint16_t idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
      if (CellTemperatures_IgnoreCell(idx)) {
        ignorecnt++;
        continue;
      }

      cellTemperaturesSum += pack_status->cell_temperatures_dC[idx];

      if (pack_status->cell_temperatures_dC[idx] > maxCellTemperature) {
        maxCellTemperature         = pack_status->cell_temperatures_dC[idx];
        maxCellTemperaturePosition = idx;
      }

      if (pack_status->cell_temperatures_dC[idx] < minCellTemperature) {
        minCellTemperature         = pack_status->cell_temperatures_dC[idx];
        minCellTemperaturePosition = idx;
      }
    }
  }

  // update pack_status
  pack_status->max_cell_temp_dC = maxCellTemperature;
  pack_status->min_cell_temp_dC = minCellTemperature;

  pack_status->avg_cell_temp_dC =
      cellTemperaturesSum / (num_modules * MAX_THERMISTORS_PER_MODULE - ignorecnt);
  pack_status->max_cell_temp_position = maxCellTemperaturePosition;
  pack_status->min_cell_temp_position = minCellTemperaturePosition;

  pack_status->variance_cell_temp_dC = 0;
  for (uint8_t module = 0; module < num_modules; module++) {
    // 255 * 24 < UINT16_MAX so this is safe
    uint16_t start = module * MAX_THERMISTORS_PER_MODULE;

    for (uint16_t idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
      if (CellTemperatures_IgnoreCell(idx)) continue;

      int16_t diff = pack_status->cell_temperatures_dC[idx] - pack_status->avg_cell_temp_dC;
      pack_status->variance_cell_temp_dC += diff * diff;
    }
  }

  // Error checks for cell temperatures
  if (pack_status->max_cell_temp_dC > MAX_CELL_TEMP_dC) {
    Error_Present(ERROR_CELL_OVER_TEMP);
  } else {
    Error_Clear(ERROR_CELL_OVER_TEMP);
  }
}

void CellTemperatures_GetOffsets(int16_t target, int16_t *input, int16_t *output) {
  for (uint8_t module = 0; module < NUM_MODULES; module++) {
    uint16_t start = module * MAX_THERMISTORS_PER_MODULE;
    for (uint16_t idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
      output[idx] = target - input[idx];
    }
  }
}

/**************************************************************************************
 * Private Functions
 * ***********************************************************************************/
void getThermistorTemperatures(uint32_t *gpioVoltages, int16_t *temperatures, uint8_t num_modules) {
  for (uint8_t i = 0; i < num_modules; i++) {
    // calculate temperature using linear curve fit relating thermistor voltages
    // (in mV) to temperatures in (dC)
    temperatures[i] = (gpioVoltages[i * LTC6804_GPIO_COUNT] >> SHIFT_VOLTAGE) + A0;

#ifdef DEBUG_PRINT
    Board_Print_BLOCKING("Temp at module: ");
    Board_PrintNum(i, 10);
    Board_Print_BLOCKING(" is ");
    Board_PrintNum(temperatures[i], 10);
    Board_Print_BLOCKING("\n");
#endif
  }
}
