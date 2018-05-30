#ifndef _STATE_H_
#define _STATE_H_

#include "state_csb.h"
#include "state_error.h"

void init_states(void);
void advance_states(void);

typedef struct {
  uint16_t current;
  uint16_t voltage;
} Sensor_Readings_T;

typedef struct {
  bool L_contactor_closed;
  uint32_t min_cell_voltage;
  uint32_t max_cell_voltage;
} BMS_State_T;

Sensor_Readings_T sensor_readings;
BMS_State_T bms_state;

#endif // _STATE_H_
