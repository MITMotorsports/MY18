// ltc-battery-management-system
#include "cell_temperatures.h"
#include "state_types.h"
#include "config.h"
#include "board.h"

// lpc11cx4-library
#include "lpc_types.h"

/**************************************************************************************
 * Public Functions
 * ***********************************************************************************/

#ifdef FSAE_DRIVERS

void CellTemperatures_UpdateCellTemperaturesArray(uint32_t          *gpioVoltages,
                                                  uint8_t            currentThermistor,
                                                  BMS_PACK_STATUS_T *pack_status,
                                                  uint8_t            num_modules)
{
  int16_t thermistorTemperatures[MAX_NUM_MODULES];

  getThermistorTemperatures(gpioVoltages, thermistorTemperatures, num_modules);

  uint8_t i;

  for (i = 0; i < MAX_NUM_MODULES; i++) {
    pack_status->cell_temperatures_dC[i * MAX_THERMISTORS_PER_MODULE
                                      + currentThermistor] =
      thermistorTemperatures[i];
  }
}

void CellTemperatures_UpdateMaxMinAvgCellTemperatures(
  BMS_PACK_STATUS_T *pack_status,
  uint8_t            num_modules) {
  int16_t  maxCellTemperature         = INT16_MIN;
  int16_t  minCellTemperature         = INT16_MAX;
  int32_t  cellTemperaturesSum        = 0;
  uint16_t maxCellTemperaturePosition = 0;
  uint16_t minCellTemperaturePosition = 0;

  uint8_t module;

  for (module = 0; module < num_modules; module++) {
    // 255 * 24 < UINT16_MAX so this is safe
    uint16_t start = module * MAX_THERMISTORS_PER_MODULE;
    uint16_t idx;

    for (idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
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
    cellTemperaturesSum / (num_modules * MAX_THERMISTORS_PER_MODULE);
  pack_status->max_cell_temp_position = maxCellTemperaturePosition;
  pack_status->min_cell_temp_position = minCellTemperaturePosition;
}

/**************************************************************************************
 * Private Functions
 * ***********************************************************************************/
void getThermistorTemperatures(uint32_t *gpioVoltages,
                               int16_t  *thermistorTemperatures,
                               uint8_t   num_modules) {
  uint8_t i;

  for (i = 0; i < num_modules; i++) {
    // calculate temperature using linear curve fit relating thermistor voltages
    // (in mV) to temperatures in (dC)
    thermistorTemperatures[i] =
      (gpioVoltages[i * LTC6804_GPIO_COUNT] >> SHIFT_VOLTAGE) + A0;
  }
}

#endif /* ifdef FSAE_DRIVERS */
