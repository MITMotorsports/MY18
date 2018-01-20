#ifndef _STATE_TYPES_H
#define _STATE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "pack.h"


typedef enum BMS_SSM_MODE {
    BMS_SSM_MODE_INIT,
    BMS_SSM_MODE_STANDBY,
} BMS_SSM_MODE_T;


typedef struct BMS_INPUT{
    BMS_PACK_STATUS_T *pack_status;
    BMS_SSM_MODE_T mode_request;

    bool eeprom_packconfig_read_done;
    bool ltc_packconfig_check_done;

    bool state;
    bool fan_override;
} BMS_INPUT_T;

typedef enum {
    BMS_INIT_OFF,
    BMS_INIT_READ_PACKCONFIG,
    BMS_INIT_CHECK_PACKCONFIG,
    BMS_INIT_DONE
} BMS_INIT_MODE_T;

typedef struct BMS_STATE {
    BMS_SSM_MODE_T curr_mode;

    BMS_PACK_CONFIG_T *pack_config;

    BMS_INIT_MODE_T init_state;
} BMS_STATE_T;


typedef struct BMS_OUTPUT {
    bool read_eeprom_packconfig;
    bool check_packconfig_with_ltc;
} BMS_OUTPUT_T;
#endif
