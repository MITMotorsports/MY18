#ifndef _CONSOLE_TYPES_H
#define _CONSOLE_TYPES_H

typedef enum {
  C_GET,
  C_SET,
  C_HELP,
  C_CONFIG,
  C_BAL,
  C_CHRG,
  C_DIS,
  C_CONFIG_DEF,
  C_MEASURE,
  C_INDUCE_ERROR,
  NUMCOMMANDS
} command_label_t;

typedef enum {
  RWL_cell_min_mV,
  RWL_cell_max_mV,
  RWL_cell_capacity_cAh,
  RWL_num_modules,
  RWL_module_cell_count, // need to think through how this will work
  RWL_cell_charge_c_rating_cC,
  RWL_bal_on_thresh_mV,
  RWL_bal_off_thresh_mV,
  RWL_pack_cells_p,
  RWL_cv_min_current_mA,
  RWL_cv_min_current_ms,
  RWL_cc_cell_voltage_mV,
  RWL_cell_discharge_c_rating_cC,
  RWL_max_cell_temp_dC,
  RWL_soc,
  RWL_LENGTH
} rw_loc_label_t;

#define ROL_FIRST RWL_LENGTH

typedef enum {
  ROL_state = (int)ROL_FIRST,
  ROL_cell_voltages_mV,
  ROL_pack_cell_max_mV,
  ROL_pack_cell_min_mV,
  ROL_pack_current_mA,
  ROL_pack_voltage_mV,
  ROL_max_cell_temp_dC,
  ROL_soc,
  ROL_error,
  ROL_LENGTH
} ro_loc_label_t;

#endif // ifndef _CONSOLE_TYPES_H
