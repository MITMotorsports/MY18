#ifndef _STATE_TYPES_H
#define _STATE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "pack.h"


typedef enum BMS_SSM_MODE {
  BMS_SSM_MODE_INIT,
  BMS_SSM_MODE_STANDBY,
  BMS_SSM_MODE_CHARGE,
  BMS_SSM_MODE_BALANCE,
  BMS_SSM_MODE_DISCHARGE,
} BMS_SSM_MODE_T;


typedef struct BMS_INPUT {
  BMS_PACK_STATUS_T *pack_status;
  BMS_SSM_MODE_T     mode_request;
  BMS_SSM_MODE_T     vcu_mode_request;
  BMS_SSM_MODE_T     csb_mode_request;

  bool vcu_switch;

  bool contactor_weld_one; // TODO: Move to proper location in status.
  bool contactor_weld_two;
  bool contactors_closed;

  bool     ltc_packconfig_check_done;
  bool     eeprom_read_error;
  bool     fan_override;
  uint32_t msTicks;
} BMS_INPUT_T;


typedef enum {
  BMS_INIT_OFF,
  BMS_INIT_READ_PACKCONFIG,
  BMS_INIT_CHECK_PACKCONFIG,
  BMS_INIT_DONE
} BMS_INIT_MODE_T;


static const char *const BMS_INIT_MODE_NAMES[] = {
  "BMS_INIT_OFF",
  "BMS_INIT_READ_PACKCONFIG",
  "BMS_INIT_CHECK_PACKCONFIG",
  "BMS_INIT_DONE",
};


typedef enum {
  BMS_DISCHARGE_OFF,
  BMS_DISCHARGE_INIT,
  BMS_DISCHARGE_RUN,
  BMS_DISCHARGE_DONE,
} BMS_DISCHARGE_MODE_T;


static const char *const BMS_DISCHARGE_MODE_NAMES[] = {
  "BMS_DISCHARGE_OFF",
  "BMS_DISCHARGE_INIT",
  "BMS_DISCHARGE_RUN",
  "BMS_DISCHARGE_DONE",
};


typedef enum {
  BMS_CHARGE_OFF,
  BMS_CHARGE_INIT,
  BMS_CHARGE_BAL,
  BMS_CHARGE_DONE,
} BMS_CHARGE_MODE_T;


typedef struct BMS_STATE {
  BMS_SSM_MODE_T       curr_mode;
  BMS_INIT_MODE_T      init_state;
  BMS_DISCHARGE_MODE_T discharge_state;
  BMS_CHARGE_MODE_T    charge_state;
  BMS_PACK_CONFIG_T   *pack_config;
} BMS_STATE_T;


typedef struct BMS_OUTPUT {
  bool  close_contactors;
  bool *balance_req;
  bool  ltc_deinit;
  bool  check_packconfig_with_ltc;
} BMS_OUTPUT_T;


#endif // ifndef _STATE_TYPES_H
