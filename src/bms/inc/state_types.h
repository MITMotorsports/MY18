#ifndef _STATE_TYPES_H
#define _STATE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "pack.h"

typedef struct BMS_INPUT{
    BMS_PACK_STATUS_T *pack_status;
} BMS_INPUT_T;
