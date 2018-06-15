#include "pack.h"

void pushCircBuf(CellCircBuf* buf, CellValue data) {
  buf->front = (buf->front + 1) % LEN(buf->data);
  buf->data[buf->front] = data;
}

const uint16_t Pack_Config_Total_Cell_Count(BMS_PACK_CONFIG_T *config) {
  uint16_t total = 0;

  uint8_t i = 0;

  for (i = 0; i < config->num_modules; i++) {
    total += config->module_cell_count[i];
  }

  return total;
}
