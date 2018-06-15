#include "pack.h"

bool insert_sort(int n, CellValue arr[n], CellValue data, bool max) {
  for (int i = 0; i < n; ++i) {
    if (max) { if (data.val < arr[i].val) continue; }
    else     { if (data.val > arr[i].val) continue; }

    arr[i] = data;
    return true;
  }

  return false;
}

const uint16_t Pack_Config_Total_Cell_Count(BMS_PACK_CONFIG_T *config) {
  uint16_t total = 0;

  uint8_t i = 0;

  for (i = 0; i < config->num_modules; i++) {
    total += config->module_cell_count[i];
  }

  return total;
}
