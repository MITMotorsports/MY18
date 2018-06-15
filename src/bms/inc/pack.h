#ifndef _PACK_H_
#define _PACK_H_

#include <stdint.h>
#include <stdbool.h>

// Default Pack Configuration
#define CELL_MIN_mV 2500  // from datasheet, contact elliot
#define CELL_MAX_mV 4250  // from datasheet, contact elliot
#define CELL_CAPACITY_cAh 250
#define NUM_MODULES 6
#define CELL_CHARGE_C_RATING_cC 5
#define BALANCE_ON_THRESHOLD_mV 4
#define BALANCE_OFF_THRESHOLD_mV 1
#define PACK_CELLS_PARALLEL 12
#define CV_MIN_CURRENT_mA 100
#define CV_MIN_CURRENT_ms 60000
#define CC_CELL_VOLTAGE_mV 4300
#define CELL_DISCHARGE_C_RATING_cC 200  // at 27 degrees C
#define MAX_CELL_TEMP_dC 600
#define MODULE_CELL_COUNT 12
#define MIN_CELL_TEMP_dC -30
#define FAN_ON_THRESHOLD_dC 450
#define MAX_CHARGE 7100

#define LEN(arr) (sizeof((arr)) / (sizeof((arr)[0])))
typedef struct CellValue {
  uint16_t idx;  // 0-MAX_NUM_MODULES*MAX_THERMISTORS_PER_MODULE
  int16_t  val;
} CellValue;

bool insert_sort(int n, CellValue arr[n], CellValue data, bool max);

typedef struct BMS_PACK_CONFIG {
  uint32_t cell_min_mV;              // minimum cell voltage (millivolts)
  uint32_t cell_max_mV;              // maximum cell voltage (millivolts)
  uint32_t cell_capacity_cAh;        // cell capacity (centiamp hours)
  uint32_t num_modules;              // number of modules
  uint32_t cell_charge_c_rating_cC;  // charging C rating (centi-C)
  uint32_t bal_on_thresh_mV;         // balancing on threshold (millivolts)
  uint32_t bal_off_thresh_mV;        // balancing off threshold (millivolts)
  uint32_t pack_cells_p;             // pack cells parallel
  uint32_t cv_min_current_mA;        // constant voltage minimum current
                                     // (milliamps)
  uint32_t cv_min_current_ms;        // constant voltage time under minimum
                                     // current (millisecond)
  uint32_t cc_cell_voltage_mV;       // constant current cell voltage
  // Size = 4*11 = 44 bytes

  uint32_t cell_discharge_c_rating_cC;  // discharge C rating (centi-C) at 27
                                        // degrees C
  uint32_t max_cell_temp_dC;            // maximum cell temperature (decicelsius)
  uint32_t min_cell_temp_dC;            // minimum cell temperature (decicelsius)
  uint8_t *module_cell_count;           // array of cell counts for each module
  // Total Size = 44 + 4 + 4 + 1= 53 bytes

  int16_t *cell_temperature_offsets_dC;  // array of cell temperature offsets (decicelsius)
                                         // array size = #modules * thermistors/module

  int16_t fan_on_threshold_dC;  // threshold for turning fan on
                                // (decicelsius)
} BMS_PACK_CONFIG_T;

typedef struct BMS_PACK_STATUS {
  uint32_t *cell_voltages_mV;  // array of cell voltages (millivolts)
                               // array size = #modules * cells/module

  int16_t *cell_temperatures_dC;  // array of cell temperatures (decicelsius)
                                  // array size = #modules * thermistors/module

  uint32_t pack_cell_max_mV;  // maximum cell voltage (millivolts)
  uint32_t pack_cell_min_mV;  // minimum cell voltage (millivolts)
  uint32_t pack_current_mA;   // charging pack current reported by BRUSA
                              // (milliamps)
  uint32_t pack_voltage_mV;   // charging pack voltage reported by BRUSA
                              // (millivolts)
  int16_t pack_energy_wH;     // energy used in watthours

  // FSAE specific pack status variables
  CellValue max_cell_temp_dC[3];     // maximum cell temperature (decicelsius)
  CellValue min_cell_temp_dC[3];     // minimum cell temperature (decicelsius)
  int16_t avg_cell_temp_dC;         // average cell temperature (decicelsius)
  uint16_t variance_cell_temp;    // variance from the mean cell temperature (dC * dC)
  uint8_t state_of_charge;          // range 0 - 100, percentage of charge

  uint32_t pack_voltage_sum_mV;  // sum of all the voltages of every cell
} BMS_PACK_STATUS_T;

const uint16_t Pack_Config_Total_Cell_Count(BMS_PACK_CONFIG_T *config);

#endif  // ifndef _PACK_H_
