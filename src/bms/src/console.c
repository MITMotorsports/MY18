#include <string.h>
#include <stdlib.h>
#include "console.h"
#include "board.h"
#include "microrl.h"
#include "console_types.h"
#include "error_handler.h"
#include "soc.h"

/***************************************
        Private Variables
 ****************************************/

static BMS_INPUT_T *bms_input;
static BMS_STATE_T *bms_state;
static CONSOLE_OUTPUT_T *console_output;

/***************************************
        Private Functions
 ****************************************/

// [TODO] Fix to not parse strings falsely
uint32_t my_atou(const char *str) {
  uint32_t res = 0; // Initialize result

  // Iterate through all characters of input string and
  // update result
  uint32_t i;

  for (i = 0; str[i] != '\0'; ++i) res = res * 10 + str[i] - '0';

  // return result.
  return res;
}

uint32_t get_command_index(const char *name, bool *found) {
  if (found) *found = false;

  uint32_t cmd_idx       = 0;
  uint32_t cmd_arraysize = sizeof(commands) / sizeof(commands[0]);

  for (cmd_idx = 0; cmd_idx < cmd_arraysize; cmd_idx++) {
    if (strcmp(name, commands[cmd_idx].name) == 0) {
      if (found) *found = true;
      break;
    }
  }

  return cmd_idx;
}

static void get(const char *const *argv) {
  rw_loc_label_t rwloc;
  uint8_t i;

  // loop over r/w entries
  bool foundloc = false;

  for (rwloc = 0; rwloc < RWL_LENGTH; ++rwloc) {
    if (strcmp(argv[1], locstring[rwloc]) == 0) {
      foundloc = true;
      break;
    }
  }

  if (foundloc) {
    char tempstr[20];

    switch (rwloc) {
    case RWL_cell_min_mV:
      utoa(bms_state->pack_config->cell_min_mV, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_cell_max_mV:
      utoa(bms_state->pack_config->cell_max_mV, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_cell_capacity_cAh:
      utoa(bms_state->pack_config->cell_capacity_cAh, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_num_modules:
      utoa(bms_state->pack_config->num_modules, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_module_cell_count:

      for (i = 0; i < bms_state->pack_config->num_modules; i++) {
        utoa(bms_state->pack_config->module_cell_count[i],
             tempstr, 10);
        Board_Println(tempstr);
      }
      break;

    case RWL_cell_charge_c_rating_cC:
      utoa(bms_state->pack_config->cell_charge_c_rating_cC, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_bal_on_thresh_mV:
      utoa(bms_state->pack_config->bal_on_thresh_mV, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_bal_off_thresh_mV:
      utoa(bms_state->pack_config->bal_off_thresh_mV, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_pack_cells_p:
      utoa(bms_state->pack_config->pack_cells_p, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_cv_min_current_mA:
      utoa(bms_state->pack_config->cv_min_current_mA, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_cv_min_current_ms:
      utoa(bms_state->pack_config->cv_min_current_ms, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_cc_cell_voltage_mV:
      utoa(bms_state->pack_config->cc_cell_voltage_mV, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_cell_discharge_c_rating_cC:
      utoa(bms_state->pack_config->cell_discharge_c_rating_cC, tempstr, 10);
      Board_Println(tempstr);
      break;

    case RWL_max_cell_temp_dC:
      utoa(bms_input->pack_status->max_cell_temp_dC[0].val, tempstr, 10);
      Board_Println(tempstr);
      break;
    case RWL_soc:
      break;
    case RWL_LENGTH:
      break;
    }
  }
  else {
    // loop over r/o entries
    ro_loc_label_t roloc;

    for (roloc = (ro_loc_label_t)ROL_FIRST; roloc < ROL_LENGTH; ++roloc) {
      if (strcmp(argv[1], locstring[roloc]) == 0) {
        foundloc = true;
        break;
      }
    }

    if (foundloc) {
      char tempstr[20];
      uint32_t i, j, idx;
      const ERROR_STATUS_T *error_status_vector;

      switch (roloc) {
      case ROL_state:

        /*
           Board_Println(BMS_SSM_MODE_NAMES[bms_state->curr_mode]);
           Board_Println(BMS_INIT_MODE_NAMES[bms_state->init_state]);
           Board_Println(BMS_CHARGE_MODE_NAMES[bms_state->charge_state]);
           Board_Println(BMS_DISCHARGE_MODE_NAMES[bms_state->discharge_state]);
         */
        break;

      case ROL_cell_voltages_mV:
        idx = 0;

        for (i = 0; i < bms_state->pack_config->num_modules; i++) {
          Board_Print_BLOCKING("\nModule ");
          Board_PrintNum_BLOCKING(i, 10);
          Board_Println_BLOCKING(":");
          for (j = 0; j < bms_state->pack_config->module_cell_count[i]; j++) {
            Board_PrintNum_BLOCKING(bms_input->pack_status->cell_voltages_mV[idx], 10);
            Board_Print_BLOCKING("\n");
            idx++;
          }
          Board_Print_BLOCKING("\n");
        }
        Board_Print_BLOCKING("\n");
        break;

      case ROL_pack_cell_max_mV:
        utoa(bms_input->pack_status->pack_cell_max_mV, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_pack_cell_min_mV:
        utoa(bms_input->pack_status->pack_cell_min_mV, tempstr, 10);
        Board_Println(tempstr);
        break;

      // case ROL_pack_current_mA:
      //   utoa(bms_input->pack_status->pack_current_mA, tempstr, 10);
      //   Board_Println(tempstr);
      //   break;

      case ROL_p_cell_volt_sum:
        utoa(bms_input->pack_status->pack_voltage_sum_mV, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_p_cell_temp_max:
        utoa(bms_input->pack_status->max_cell_temp_dC[0].val, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_p_cell_temp_min:
        utoa(bms_input->pack_status->min_cell_temp_dC[0].val, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_p_cell_temp_avg:
        utoa(bms_input->pack_status->avg_cell_temp_dC, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_p_cell_temp_var:
        utoa(bms_input->pack_status->variance_cell_temp, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_p_cell_temp:
        for (uint8_t module = 0; module < bms_state->pack_config->num_modules; module++) {
          Board_Print_BLOCKING("Module ");
          Board_PrintNum_BLOCKING(module, 10);
          Board_Println_BLOCKING(":");

          uint16_t start = module * MAX_THERMISTORS_PER_MODULE;
          for (uint16_t idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
            Board_PrintNum_BLOCKING(bms_input->pack_status->cell_temperatures_dC[idx], 10);
            Board_Print_BLOCKING("\n");
          }
          Board_Print_BLOCKING("\n");
        }
        Board_Print_BLOCKING("\n");
        break;

      case ROL_soc:
        utoa(bms_input->pack_status->state_of_charge, tempstr, 10);
        Board_Println(tempstr);
        break;

      case ROL_error:
        error_status_vector = Error_GetStatus(0);

        for (i = 0; i < ERROR_NUM_ERRORS; ++i)
        {
          if (error_status_vector[i].handling || error_status_vector[i].error) {
            Board_Println(ERROR_NAMES[i]);
          }
        }
        break;

      case ROL_LENGTH:
        break; // how the hell?
      }
    }
    else {
      Board_Println("invalid get location");
    }
  }
}

// [TODO] Check max/min bounds and max > min
static void set(const char *const *argv) {
  // if (bms_state->curr_mode != BMS_SSM_MODE_STANDBY) {
  //   Board_Println("Set failed (not in standby mode)!");
  //   return;
  // }
  rw_loc_label_t rwloc;

  // loop over r/w entries
  bool foundloc = false;

  for (rwloc = 0; rwloc < RWL_LENGTH; ++rwloc) {
    if (strcmp(argv[1], locstring[rwloc]) == 0) {
      foundloc = true;
      break;
    }
  }

  if (foundloc) {
    // uint8_t ret;
    switch (rwloc) {
    case RWL_soc:
      bms_input->pack_status->state_of_charge = 100;
      SOC_Max(bms_input->pack_status);
      Board_Println("Set state of charge to 100");
      break;
    case RWL_cell_min_mV:
      bms_state->pack_config->cell_min_mV = my_atou(argv[2]);
      Board_Print("Set min cell voltage to ");
      Board_PrintNum(bms_state->pack_config->cell_min_mV, 10);
      Board_Print(" mV\n");
      break;
    case RWL_cell_max_mV:
      bms_state->pack_config->cell_max_mV = my_atou(argv[2]);
      Board_Print("Set max cell voltage to ");
      Board_PrintNum(bms_state->pack_config->cell_max_mV, 10);
      Board_Print(" mV\n");
      break;
    case RWL_max_cell_temp_dC:
      bms_state->pack_config->max_cell_temp_dC = my_atou(argv[2]);
      Board_Print("Set max cell temperature to ");
      Board_PrintNum(bms_state->pack_config->max_cell_temp_dC, 10);
      Board_Print(" dC\n");
      break;
    default:
      Board_Println("Set failed (command not yet implemented?)!");
      break;
    }
    // ret = EEPROM_ChangeConfig(rwloc,my_atou(argv[2]));
    // if (ret != 0) {
    //   Board_Println("Set failed (command not yet implemented?)!");
    // }
  } else {
    // loop over r/o entries
    ro_loc_label_t roloc;

    for (roloc = (ro_loc_label_t)ROL_FIRST; roloc < ROL_LENGTH; ++roloc) {
      if (strcmp(argv[1], locstring[roloc]) == 0) {
        foundloc = true;
        Board_Println("this location is read only");
        return;
      }
    }
    Board_Println("invalid location");
  }
}

static void help(const char *const *argv) {
  bool found;

  uint32_t cmd_idx = get_command_index(argv[1], &found);

  if (!found) {
    Board_Println("Unknown command!");
    return;
  }

  Board_Println_BLOCKING(commands[cmd_idx].help);

  Board_Print("");

  command_label_t label = commands[cmd_idx].label;

  if ((label == C_GET) || (label == C_SET)) {
    rw_loc_label_t i;
    Board_Println_BLOCKING("------r/w entries------");

    for (i = 0; i < RWL_LENGTH; ++i) {
      Board_Println_BLOCKING(locstring[i]); // blocking print.
    }

    Board_Println_BLOCKING("------r/o entries------");

    for (i = ROL_FIRST; i < (rw_loc_label_t)(ROL_LENGTH); ++i) {
      Board_Println_BLOCKING(locstring[i]); // blocking print.
    }
  }
}

// [TODO] This might not be safe
static void config(const char *const *argv) {
  UNUSED(argv);

  // if (bms_state->curr_mode == BMS_SSM_MODE_STANDBY) {
  //   bms_state->curr_mode  = BMS_SSM_MODE_INIT;
  //   bms_state->init_state = BMS_INIT_OFF;
  // }
}

static void measure(const char *const *argv) {
  if (strcmp(argv[1], "on") == 0) {
    console_output->measure_on = true;
    Board_Println("Measure On!");
  } else if (strcmp(argv[1], "off") == 0) {
    console_output->measure_on = false;
    Board_Println("Measure Off!");
  } else if (strcmp(argv[1], "print_flags") == 0) {
    if (console_output->measure_temp) {
      Board_Println("Cell Temps: On");
    } else {
      Board_Println("Cell Temps: Off");
    }

    if (console_output->measure_voltage) {
      Board_Println("Cell Voltages: On");
    } else {
      Board_Println("Cell Voltages: Off");
    }

    if (console_output->measure_packvoltage) {
      Board_Println("Pack Voltage: On");
    } else {
      Board_Println("Pack Voltage: Off");
    }

  } else if (strcmp(argv[1], "temps") == 0) {
    console_output->measure_temp = !console_output->measure_temp;
  } else if (strcmp(argv[1], "voltages") == 0) {
    console_output->measure_voltage = !console_output->measure_voltage;
  } else if (strcmp(argv[1], "packvoltage") == 0) {
    console_output->measure_packvoltage = !console_output->measure_packvoltage;
  } else {
    Board_Println("Unrecognized command!");
  }
}

static void bal(const char *const *argv) {
  UNUSED(argv);

  // if ((bms_state->curr_mode == BMS_SSM_MODE_STANDBY) ||
  //     (bms_state->curr_mode == BMS_SSM_MODE_BALANCE)) {
  //   if (strcmp(argv[1], "off") == 0) {
  //     console_output->valid_mode_request = false;
  //     console_output->balance_mV         = UINT32_MAX;
  //     Board_Println("bal off");
  //   } else {
  //     console_output->valid_mode_request = true;
  //     console_output->mode_request       = BMS_SSM_MODE_BALANCE;
  //     console_output->balance_mV         = my_atou(argv[1]);
  //     Board_Println("bal on");
  //   }
  // } else {
  //   Board_Println("Must be in standby");
  // }
}

static void chrg(const char *const *argv) {
  UNUSED(argv);

  // if ((bms_state->curr_mode == BMS_SSM_MODE_STANDBY) ||
  //     (bms_state->curr_mode == BMS_SSM_MODE_CHARGE)) {
  //   if (console_output->valid_mode_request) {
  //     console_output->valid_mode_request = false;
  //     Board_Println("chrg off");
  //   } else {
  //     console_output->valid_mode_request = true;
  //     console_output->mode_request       = BMS_SSM_MODE_CHARGE;
  //     Board_Println("chrg on");
  //   }
  // } else {
  //   Board_Println("Must be in standby");
  // }
}

static void dis(const char *const *argv) {
  UNUSED(argv);

  // if ((bms_state->curr_mode == BMS_SSM_MODE_STANDBY) ||
  //     (bms_state->curr_mode == BMS_SSM_MODE_DISCHARGE)) {
  //   if (console_output->valid_mode_request) {
  //     console_output->valid_mode_request = false;
  //     Board_Println("dis off");
  //   } else {
  //     console_output->valid_mode_request = true;
  //     console_output->mode_request       = BMS_SSM_MODE_DISCHARGE;
  //     Board_Println("dis on");
  //   }
  // } else {
  //   Board_Println("Must be in standby");
  // }
}

struct command_handler_pair {
  command_label_t         label;
  COMMAND_EXECUTE_HANDLER handler;
};

static const struct command_handler_pair handlers[] = {
  { C_GET,  get  }, { C_SET, set }, { C_HELP,    help    }, { C_CONFIG, config }, { C_BAL, bal },
  { C_CHRG, chrg }, { C_DIS, dis }, { C_MEASURE, measure }
};

/***************************************
        Public Functions
 ****************************************/
void console_init(BMS_INPUT_T *input, BMS_STATE_T *state, CONSOLE_OUTPUT_T *con_output) {
  bms_input                          = input;
  bms_state                          = state;
  console_output                     = con_output;
  // console_output->valid_mode_request = false;
  // console_output->mode_request       = BMS_SSM_MODE_STANDBY;

  console_output->measure_on          = false;
  console_output->measure_temp        = false;
  console_output->measure_voltage     = false;
  console_output->measure_packvoltage = false;
}

void executerl(int32_t argc, const char *const *argv) {
  bool found_command = false;

  uint32_t cmd_idx = get_command_index(argv[0], &found_command);

  if (found_command) {
    if (commands[cmd_idx].nargs == (uint32_t)(argc - 1)) {
      uint32_t handler_idx;
      uint32_t handler_count = sizeof(handlers) / sizeof(handlers[0]);

      for (handler_idx = 0; handler_idx < handler_count; handler_idx++) {
        if (handlers[handler_idx].label == commands[cmd_idx].label) {
          handlers[handler_idx].handler(argv);
        }
      }
    } else {
      Board_Println("incorrect number of args");
    }
  }
  else {
    Board_Println("Unrecognized command");
  }
}
