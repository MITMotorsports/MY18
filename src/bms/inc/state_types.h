#ifndef _STATE_TYPES_H
#define _STATE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "pack.h"


typedef enum BMS_SSM_MODE {
    BMS_SSM_MODE_INIT,
    BMS_SSM_MODE_STANDBY,
} BMS_SSM_MODE_T;

typedef struct BMS_STATE {
    BMS_SSM_MODE_T curr_mode;
} BMS_STATE_T;
typedef struct BMS_INPUT{
    BMS_PACK_STATUS_T *pack_status;
    BMS_SSM_MODE_T mode_request;

    bool state;
    bool fan_override;
} BMS_INPUT_T;

#endif
