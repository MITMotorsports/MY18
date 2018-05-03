#ifndef __FAULT_HEARTBEATS_H
#define __FAULT_HEARTBEATS_H

#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f2xx_hal.h"

#include "extern.h"


// CONTAINER DECLARATION
typedef struct {
  uint32_t fcn;
  uint32_t bms;
  uint32_t mc;
  uint32_t current_sensor;
} Heartbeats_T;


extern volatile Heartbeats_T heartbeats;


bool any_recoverable_heartbeat_faults(void);

bool check_BMS_heartbeat_bad(void);
bool check_FCN_heartbeat_bad(void);
bool check_MC_heartbeat_bad(void);
bool check_CS_heartbeat_bad(void);

#endif // ifndef __FAULT_HEARTBEATS_H
