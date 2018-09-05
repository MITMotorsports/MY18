#ifndef CELL_TEMPERATURES_H
#define CELL_TEMPERATURES_H

// ltc-battery-management-system
#include "state_types.h"
#include "config.h"

// constants
#define NUMBER_OF_MULTIPLEXER_LOGIC_CONTROL_INPUTS 5
#define THERMISTOR_GROUP_ONE_START    0
#define THERMISTOR_GROUP_ONE_END      6
#define THERMISTOR_GROUP_ONE_OFFSET   3
#define THERMISTOR_GROUP_TWO_START    7
#define THERMISTOR_GROUP_TWO_END      7
#define THERMISTOR_GROUP_TWO_OFFSET   5
#define THERMISTOR_GROUP_THREE_START  8
#define THERMISTOR_GROUP_THREE_END    23
#define THERMISTOR_GROUP_THREE_OFFSET 8

// Contansts for linear curve fit relating thermistor voltages (in mV) to
// temperatures
// (in dC)
// The equation is temp_dC = thermistorVoltage/4 - 39
#define SHIFT_VOLTAGE 2
#define A0 -39

/****************************************************************************************
 * Public Functions
 * *************************************************************************************/

/**
 * @details updates array of cell temperatures in pack_status with values stored
 *in
 *          gpioVoltages
 *
 * @param gpioVoltages array of voltages measured on each GPIO of the LTC6804
 *chips
 *                     gpioVoltages is structured as follows {GPIO1_module1,
 *...,
 *                     GPIO5_module1, GPIO1_module2, ..., GPIO5_module2, ...}
 * @param currentThermistor number of thermistor currently selected
 * @param pack_status mutable datatype containing array of cell temperatures
 */
void CellTemperatures_UpdateCellTemperaturesArray(uint32_t *gpioVoltages,
                                                  uint8_t currentThermistor,
                                                  BMS_PACK_CONFIG_T *pack_config,
                                                  BMS_PACK_STATUS_T *pack_status,
                                                  bool with_offsets,
                                                  uint8_t num_modules);

/**
 * @details updates maximum, minimum, and average cell temperatures in
 *pack_status
 *
 * @param pack_status mutable datatype containing array of cell temperatures,
 *maximum
 *                    cell temperature, minimum cell temperature, and average
 *cell
 *                    temperature
 */
void CellTemperatures_UpdateMaxMinAvgCellTemperatures(BMS_PACK_CONFIG_T *pack_config,
                                                      BMS_PACK_STATUS_T *pack_status,
                                                      uint8_t num_modules);

bool CellTemperatures_IgnoreCell(uint16_t cell_id);

void CellTemperatures_GetOffsets(int16_t target, int16_t *input, int16_t *output);

/****************************************************************************************
 * Private Functions
 * *************************************************************************************/

/**
 * @details creates an array of thermistor temperatures (in dC) from an array of
 *gpio
 * voltages (in mV)
 *
 * @param gpioVoltages array of voltages measured on each GPIO of the LTC6804
 *chips
 *                     gpioVoltages is structured as follows {GPIO1_module1,
 *...,
 *                     GPIO5_module1, GPIO1_module2, ..., GPIO5_module2, ...}
 * @param thermistorTemperatures mutable array of thermistor temperatures. At
 *the end
 * of the function call, thermistorTemperatures will have the following form:
 * {thermistorTemperature_GPIO1_module1, thermistorTemperature_GPIO1_module2,
 *...,
 * thermistorTemperature_GPIO1_modulen, ...}
 */
void getThermistorTemperatures(uint32_t *gpioVoltages,
                                int16_t *temperatures,
                                uint8_t num_modules);

#endif // CELL_TEMPERATURES
