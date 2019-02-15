#include "main.h"
#include "measure.h"

static char str[10];

static BMS_PACK_STATUS_T pack_status;
static BMS_INPUT_T  bms_input;
static BMS_OUTPUT_T bms_output;
static BMS_STATE_T  bms_state;

static BMS_PACK_CONFIG_T pack_config;
static uint32_t cell_voltages[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE] = {};
static int16_t  cell_temperatures[MAX_NUM_MODULES * MAX_THERMISTORS_PER_MODULE] = {};
static int16_t  cell_temperature_offsets[MAX_NUM_MODULES * MAX_THERMISTORS_PER_MODULE] = {};
static uint8_t  module_cell_count[MAX_NUM_MODULES];

// memory allocation for BMS_OUTPUT balancing
static bool balance_reqs[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];
static bool balance_waitingoff[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];
static uint32_t balance_timeon[MAX_NUM_MODULES * MAX_CELLS_PER_MODULE];

// memory for console
static microrl_t rl;
static CONSOLE_OUTPUT_T console_output;


int main(void) {
  Init_BMS_Structs();
  Board_Chip_Init();
  Board_GPIO_Init();
  Board_UART_Init(115200);
  Board_ADC_Init();
  Board_CAN_Init();
  EEPROM_Init(LPC_SSP1, EEPROM_BAUD, EEPROM_CS_PIN);
  SOC_Init(&pack_status);

  Error_Init();

  Board_ADC_Init();

  // setup readline/console
  microrl_init(&rl, Board_Print);
  microrl_set_execute_callback(&rl, executerl);
  console_init(&bms_input, &bms_state, &console_output);

  Board_Println_BLOCKING("Currently running: "HASH);
  Board_Println_BLOCKING("Flashed by: "AUTHOR);

  // Start with fault line high
  Board_Pin_Set(PIN_BMS_FAULT, true);

  Board_LTC6804_DeInit();


  //ignore PEC errors for startup
  Error_Ignore(ERROR_LTC_PEC);
  Error_Ignore(ERROR_LTC_CVST);
  Error_Ignore(ERROR_LTC_OWT);
  while (1) {
    // Setting fault pin high
    Board_Pin_Set(PIN_BMS_FAULT, GPIO_HIGH);

    Process_Keyboard();
    Process_Input(&bms_input, &bms_output);
    Process_Output(&bms_input, &bms_output, &bms_state);
    if (false){
      static uint32_t last_print = 0;
      if (msTicks - last_print > 1000) {
        Board_Print_BLOCKING("min: ");
        for (int i = 0; i < 3; ++i) {
          Board_PrintNum_BLOCKING(pack_status.min_cell_temp_dC[i].val, 10);
          Board_Print_BLOCKING(" ");
        }
        Board_Print_BLOCKING("\nmax: ");
        for (int i = 0; i < 3; ++i) {
          Board_PrintNum_BLOCKING(pack_status.max_cell_temp_dC[i].val, 10);
          Board_Print_BLOCKING(" ");
        }
        Board_Print_BLOCKING("\navg: ");
        Board_PrintNum_BLOCKING(pack_status.avg_cell_temp_dC, 10);
        Board_Print_BLOCKING("\nvar: ");
        Board_PrintNum_BLOCKING(pack_status.variance_cell_temp, 10);

        Board_Print_BLOCKING("\ntemps: {");

        for (uint8_t module = 0; module < NUM_MODULES; module++) {
          uint16_t start = module * MAX_THERMISTORS_PER_MODULE;
          for (uint16_t idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
            Board_PrintNum_BLOCKING(pack_status.cell_temperatures_dC[idx], 10);
            Board_Print_BLOCKING(", ");
          }
        }
        Board_Print_BLOCKING("}\n");
        last_print = msTicks;
      }
    }

    // Uncomment to get new offset array for a given target temperature in dC.
    // Find_Offsets(185);

    if (Error_Should_Fault()) {
      Board_Println("Requesting Halt..");
      break;
    }
  }

  Board_Pin_Set(PIN_BMS_FAULT, false);

  Board_Println("Halting...");

  while (1) {
    Board_Pin_Set(PIN_BMS_FAULT, false);
    Error_Should_Fault();
    Process_Output(&bms_input, &bms_output, &bms_state);
  }

  return 0;
}


void Process_Input(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output) {
  Board_CAN_Receive(bms_input);
  // Board_GetModeRequest(&console_output, bms_input);
  if (!bms_output->attempt_ltc_init) {
    Board_LTC6804_ProcessInputs(&pack_config, &pack_status, &bms_state);
  }

  // TODO: Reenable after Roman
  // SOC_Estimate(&pack_status);

  bms_input->H_contactor_welded = Board_Contactor_High_Welded();
  bms_input->L_contactor_welded = Board_Contactor_Low_Welded();

  bms_input->H_contactor_closed = Board_Contactor_High_Closed();
  bms_input->L_contactor_closed = Board_Contactor_Low_Closed();

  // TODO: Fix the BMS in hardware for HIGH side weld.
  // if(bms_input->H_contactor_welded != bms_input->H_contactor_closed) {
  //   Error_Present(ERROR_H_CONTACTOR_ERROR);
  //   if(bms_input->H_contactor_welded) {
  //     Error_Present(ERROR_H_CONTACTOR_WELD);
  //   }
  // } else {
  //   Error_Clear(ERROR_H_CONTACTOR_ERROR);
  //   Error_Clear(ERROR_H_CONTACTOR_WELD);
  // }
  Error_Clear(ERROR_H_CONTACTOR_ERROR);
  Error_Clear(ERROR_H_CONTACTOR_WELD);

  if(bms_input->L_contactor_welded != bms_input->L_contactor_closed) {
    // Error_Present(ERROR_L_CONTACTOR_ERROR);
    if(bms_input->L_contactor_welded) {
      // Error_Present(ERROR_L_CONTACTOR_WELD);
    }
  } else {
    Error_Clear(ERROR_L_CONTACTOR_ERROR);
    Error_Clear(ERROR_L_CONTACTOR_WELD);
  }

#ifdef DEBUG_PRINT
  static uint32_t lastpr = 0;

  if (msTicks - lastpr > 1000) {
    if (bms_input->L_contactor_closed) Board_Print_BLOCKING("\nL closed\n");

    if (bms_input->H_contactor_closed) Board_Print_BLOCKING("\nH closed\n");

    if (bms_input->L_contactor_welded) Board_Print_BLOCKING("\nL welded\n");

    if (bms_input->H_contactor_welded) Board_Print_BLOCKING("\nH welded\n");

    lastpr = msTicks;
  }
#endif /* ifdef DEBUG_PRINT */
}

void Process_Output(BMS_INPUT_T  *bms_input,
                    BMS_OUTPUT_T *bms_output,
                    BMS_STATE_T  *bms_state) {

  Board_CAN_Transmit(bms_input, bms_output);

  if (bms_output->attempt_ltc_init) {

    bms_output->attempt_ltc_init = !Board_LTC6804_Init(&pack_config,
                                                       cell_voltages);
  }
  else {
    Board_LTC6804_UpdateBalanceStates(bms_output->balance_req);
  }

  Output_Measurements(&console_output, bms_input, bms_state);
}

void Process_Keyboard(void) {
  uint32_t readln = Board_Read(str, 50);
  uint32_t i;

  for (i = 0; i < readln; i++) {
    microrl_insert_char(&rl, str[i]);
  }
}


void Init_BMS_Structs(void) {
  bms_state.pack_config = &pack_config;

  // Default pack configuration, from pack.h
  pack_config.module_cell_count           = module_cell_count;
  pack_config.cell_min_mV                 = CELL_MIN_mV;
  pack_config.cell_max_mV                 = CELL_MAX_mV;
  pack_config.cell_capacity_cAh           = CELL_CAPACITY_cAh;
  pack_config.num_modules                 = NUM_MODULES;
  pack_config.cell_charge_c_rating_cC     = CELL_CHARGE_C_RATING_cC;
  pack_config.bal_on_thresh_mV            = BALANCE_ON_THRESHOLD_mV;
  pack_config.bal_off_thresh_mV           = BALANCE_OFF_THRESHOLD_mV;
  pack_config.pack_cells_p                = PACK_CELLS_PARALLEL;
  pack_config.cv_min_current_mA           = CV_MIN_CURRENT_mA;
  pack_config.cv_min_current_ms           = CV_MIN_CURRENT_ms;
  pack_config.cc_cell_voltage_mV          = CC_CELL_VOLTAGE_mV;
  pack_config.cell_discharge_c_rating_cC  = CELL_DISCHARGE_C_RATING_cC;
  pack_config.max_cell_temp_dC            = MAX_CELL_TEMP_dC;
  pack_config.fan_on_threshold_dC         = FAN_ON_THRESHOLD_dC;
  pack_config.min_cell_temp_dC            = MIN_CELL_TEMP_dC;

  (void) Init_Offsets();
  pack_config.cell_temperature_offsets_dC = cell_temperature_offsets;

  for (size_t i = 0; i < MAX_NUM_MODULES; i++) {
    pack_config.module_cell_count[i] = MODULE_CELL_COUNT;
  }

  bms_input.pack_status        = &pack_status;

  bms_input.H_contactor_welded = false;
  bms_input.L_contactor_welded = false;

  bms_input.H_contactor_closed = false;
  bms_input.L_contactor_closed = false;

  bms_input.ltc_packconfig_check_done = false;
  bms_input.eeprom_read_error         = false;

  bms_output.attempt_ltc_init = true;
  bms_output.balance_req = balance_reqs;
  memset(balance_reqs, 0, sizeof(balance_reqs));

  pack_status.cell_voltages_mV     = cell_voltages;
  pack_status.cell_temperatures_dC = cell_temperatures;
  pack_status.pack_cell_max_mV     = 0;
  pack_status.pack_cell_min_mV     = 0xFFFFFFFF;
  pack_status.pack_current_mA      = 0;
  pack_status.pack_voltage_mV      = 0;

  pack_status.avg_cell_temp_dC = 0;
}

void Find_Offsets(int16_t target) {
  static uint32_t last_print = 0;
  if (msTicks - last_print > 1000) {
    memset(cell_temperature_offsets, 0, sizeof(cell_temperature_offsets));
    CellTemperatures_GetOffsets(target,
                                pack_status.cell_temperatures_dC,
                                cell_temperature_offsets);
    Board_Print_BLOCKING("\ntemps: {");
    for (uint8_t module = 0; module < NUM_MODULES; module++) {
      uint16_t start = module * MAX_THERMISTORS_PER_MODULE;
      for (uint16_t idx = start; idx < start + MAX_THERMISTORS_PER_MODULE; idx++) {
        Board_PrintNum_BLOCKING(pack_status.cell_temperatures_dC[idx], 10);
        Board_Print_BLOCKING(", ");
      }
    }
    Board_Print_BLOCKING("}\n");

    Board_Print_BLOCKING("\noffsets: {");
    for (uint16_t idx = 0; idx < MAX_NUM_MODULES * MAX_THERMISTORS_PER_MODULE; idx++) {
      Board_PrintNum_BLOCKING(cell_temperature_offsets[idx], 10);
      Board_Print_BLOCKING(", ");
    }

    Board_Print_BLOCKING("}\n\n");

    memset(cell_temperature_offsets, 0, sizeof(cell_temperature_offsets));
    last_print = msTicks;
  }
}

// You can tell me that I could've initialized this in the declaration.
// I will ask you to try and see if it works.
void Init_Offsets(void) {
  memset(cell_temperature_offsets, 0, sizeof(cell_temperature_offsets));
  return;  // this calibration got bad
  cell_temperature_offsets[0] = -43;
  cell_temperature_offsets[1] = -44;
  cell_temperature_offsets[2] = -43;
  cell_temperature_offsets[3] = -45;
  cell_temperature_offsets[4] = -40;
  cell_temperature_offsets[5] = -45;
  cell_temperature_offsets[6] = -42;
  cell_temperature_offsets[7] = -41;
  cell_temperature_offsets[8] = -46;
  cell_temperature_offsets[9] = -45;
  cell_temperature_offsets[10] = -39;
  cell_temperature_offsets[11] = -44;
  cell_temperature_offsets[12] = -44;
  cell_temperature_offsets[13] = -43;
  cell_temperature_offsets[14] = -40;
  cell_temperature_offsets[15] = -44;
  cell_temperature_offsets[16] = -39;
  cell_temperature_offsets[17] = -43;
  cell_temperature_offsets[18] = -38;
  cell_temperature_offsets[19] = -42;
  cell_temperature_offsets[20] = -41;
  cell_temperature_offsets[21] = -45;
  cell_temperature_offsets[22] = -40;
  cell_temperature_offsets[23] = -43;
  cell_temperature_offsets[24] = -38;
  cell_temperature_offsets[25] = -39;
  cell_temperature_offsets[26] = -39;
  cell_temperature_offsets[27] = -41;
  cell_temperature_offsets[28] = -38;
  cell_temperature_offsets[29] = -43;
  cell_temperature_offsets[30] = -40;
  cell_temperature_offsets[31] = -39;
  cell_temperature_offsets[32] = -43;
  cell_temperature_offsets[33] = -41;
  cell_temperature_offsets[34] = -41;
  cell_temperature_offsets[35] = -44;
  cell_temperature_offsets[36] = -39;
  cell_temperature_offsets[37] = -45;
  cell_temperature_offsets[38] = -39;
  cell_temperature_offsets[39] = -46;
  cell_temperature_offsets[40] = -38;
  cell_temperature_offsets[41] = -43;
  cell_temperature_offsets[42] = -41;
  cell_temperature_offsets[43] = -41;
  cell_temperature_offsets[44] = -38;
  cell_temperature_offsets[45] = -44;
  cell_temperature_offsets[46] = -39;
  cell_temperature_offsets[47] = -42;
  cell_temperature_offsets[48] = -34;
  cell_temperature_offsets[49] = -32;
  cell_temperature_offsets[50] = -29;
  cell_temperature_offsets[51] = -37;
  cell_temperature_offsets[52] = -24;
  cell_temperature_offsets[53] = -34;
  cell_temperature_offsets[54] = -33;
  cell_temperature_offsets[55] = -28;
  cell_temperature_offsets[56] = -36;
  cell_temperature_offsets[57] = -38;
  cell_temperature_offsets[58] = -34;
  cell_temperature_offsets[59] = -31;
  cell_temperature_offsets[60] = -36;
  cell_temperature_offsets[61] = -32;
  cell_temperature_offsets[62] = -26;
  cell_temperature_offsets[63] = -38;
  cell_temperature_offsets[64] = -29;
  cell_temperature_offsets[65] = -31;
  cell_temperature_offsets[66] = -31;
  cell_temperature_offsets[67] = -26;
  cell_temperature_offsets[68] = -26;
  cell_temperature_offsets[69] = -39;
  cell_temperature_offsets[70] = -28;
  cell_temperature_offsets[71] = -23;
  cell_temperature_offsets[72] = -40;
  cell_temperature_offsets[73] = -42;
  cell_temperature_offsets[74] = -42;
  cell_temperature_offsets[75] = -45;
  cell_temperature_offsets[76] = -41;
  cell_temperature_offsets[77] = -46;
  cell_temperature_offsets[78] = -40;
  cell_temperature_offsets[79] = -43;
  cell_temperature_offsets[80] = -49;
  cell_temperature_offsets[81] = -43;
  cell_temperature_offsets[82] = -41;
  cell_temperature_offsets[83] = -44;
  cell_temperature_offsets[84] = -40;
  cell_temperature_offsets[85] = -48;
  cell_temperature_offsets[86] = -44;
  cell_temperature_offsets[87] = -52;
  cell_temperature_offsets[88] = -43;
  cell_temperature_offsets[89] = -48;
  cell_temperature_offsets[90] = -41;
  cell_temperature_offsets[91] = -43;
  cell_temperature_offsets[92] = -42;
  cell_temperature_offsets[93] = -48;
  cell_temperature_offsets[94] = -41;
  cell_temperature_offsets[95] = -46;
  cell_temperature_offsets[96] = -42;
  cell_temperature_offsets[97] = -44;
  cell_temperature_offsets[98] = -41;
  cell_temperature_offsets[99] = -45;
  cell_temperature_offsets[100] = -42;
  cell_temperature_offsets[101] = -47;
  cell_temperature_offsets[102] = -43;
  cell_temperature_offsets[103] = -43;
  cell_temperature_offsets[104] = -49;
  cell_temperature_offsets[105] = -46;
  cell_temperature_offsets[106] = -41;
  cell_temperature_offsets[107] = -50;
  cell_temperature_offsets[108] = -42;
  cell_temperature_offsets[109] = -49;
  cell_temperature_offsets[110] = -42;
  cell_temperature_offsets[111] = -49;
  cell_temperature_offsets[112] = -43;
  cell_temperature_offsets[113] = -47;
  cell_temperature_offsets[114] = -44;
  cell_temperature_offsets[115] = -44;
  cell_temperature_offsets[116] = -43;
  cell_temperature_offsets[117] = -47;
  cell_temperature_offsets[118] = -43;
  cell_temperature_offsets[119] = -43;
  cell_temperature_offsets[120] = -34;
  cell_temperature_offsets[121] = -34;
  cell_temperature_offsets[122] = -34;
  cell_temperature_offsets[123] = -36;
  cell_temperature_offsets[124] = -27;
  cell_temperature_offsets[125] = -36;
  cell_temperature_offsets[126] = -34;
  cell_temperature_offsets[127] = -31;
  cell_temperature_offsets[128] = -39;
  cell_temperature_offsets[129] = -34;
  cell_temperature_offsets[130] = -31;
  cell_temperature_offsets[131] = -30;
  cell_temperature_offsets[132] = -30;
  cell_temperature_offsets[133] = -36;
  cell_temperature_offsets[134] = -28;
  cell_temperature_offsets[135] = -38;
  cell_temperature_offsets[136] = -28;
  cell_temperature_offsets[137] = -34;
  cell_temperature_offsets[138] = -32;
  cell_temperature_offsets[139] = -28;
  cell_temperature_offsets[140] = -31;
  cell_temperature_offsets[141] = -37;
  cell_temperature_offsets[142] = -27;
  cell_temperature_offsets[143] = -28;
}
