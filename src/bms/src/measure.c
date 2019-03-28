#include "measure.h"
#include "main.h"
#include "board.h"

#define TEMPS_PRINT_PERIOD_ms 1000
#define VOLTAGES_PRINT_PERIOD_ms 1000

static inline void Print_CVArr(CellValue *cvarr, size_t sz) {
  for (size_t i = 0; i < sz; ++i) {
    Board_PrintNum_BLOCKING(cvarr[i].val, 10);
    Board_Print_BLOCKING(" at ");
    Board_PrintNum_BLOCKING(cvarr[i].idx, 10);
    if (i != sz - 1) Board_Print_BLOCKING(", ");
  }
}

void Output_Measurements(CONSOLE_OUTPUT_T *console_output,
                         BMS_INPUT_T *bms_input,
                         BMS_STATE_T *bms_state) {
  if (!console_output->measure_on)
    return;

  static uint32_t lastVoltagesPrintTime = 0;
  static uint32_t lastTempsPrintTime    = 0;

  if (console_output->measure_temp && (msTicks - lastTempsPrintTime) > TEMPS_PRINT_PERIOD_ms) {
    Board_Print_BLOCKING("\nMin Cell Temps: ");
    Print_CVArr(bms_input->pack_status->min_cell_temp_dC, 3);
    Board_Print_BLOCKING("\nMax Cell Temps: ");
    Print_CVArr(bms_input->pack_status->max_cell_temp_dC, 3);

    lastTempsPrintTime = msTicks;
  }

  if ((msTicks - lastVoltagesPrintTime) > VOLTAGES_PRINT_PERIOD_ms) {
    if (console_output->measure_voltage) {
      Board_Print_BLOCKING("\nMax Cell Voltage: ");
      Board_PrintNum_BLOCKING(bms_input->pack_status->pack_cell_max_mV, 10);
      Board_Print_BLOCKING("\nMin Cell Voltage: ");
      Board_PrintNum_BLOCKING(bms_input->pack_status->pack_cell_min_mV, 10);
    }

    if (console_output->measure_packvoltage) {
      Board_Print_BLOCKING("\nPack Voltage: ");
      Board_PrintNum_BLOCKING(bms_input->pack_status->pack_voltage_sum_mV, 10);
    }

    lastVoltagesPrintTime = msTicks;
  }
}
