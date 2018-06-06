#ifndef _STATE_TYPES_H
#define _STATE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "pack.h"


typedef struct BMS_INPUT {
  BMS_PACK_STATUS_T *pack_status;

  bool H_contactor_welded; // TODO: Move to proper location in status.
  bool L_contactor_welded;

  bool H_contactor_closed; // TODO: Move to proper location in status.
  bool L_contactor_closed;

  bool ltc_packconfig_check_done;
  bool eeprom_read_error;
} BMS_INPUT_T;


typedef struct BMS_STATE {
  BMS_PACK_CONFIG_T   *pack_config;
} BMS_STATE_T;


typedef struct BMS_OUTPUT {
  bool attempt_ltc_init;
  bool *balance_req;
} BMS_OUTPUT_T;


#endif // ifndef _STATE_TYPES_H
