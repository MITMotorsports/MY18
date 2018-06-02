#include "state.h"

void init_states(void) {
  init_error_state();
  init_csb_state();

  bms_state.L_contactor_closed = false;
  bms_state.min_cell_voltage   = 0;
  bms_state.max_cell_voltage   = 0;

  sensor_readings.current = 0;
  sensor_readings.voltage = 0;
}

void advance_states(void) {
  advance_error_state();

  if (!get_error_state()) {
    advance_csb_state();
  }
}
