#include "main.h"

int main(void) {
  Init_BMS_Structs();
  Board_Chip_Init();
  Board_GPIO_Init();
  Board_UART_Init(57600);
  Board_ADC_Init();
  Board_CAN_Init();
  EEPROM_Init(LPC_SSP1, EEPROM_BAUD, EEPROM_CS_PIN);
  I2C_Init();
  Thermostat_Init();

  SOC_Init(&pack_status);

  Error_Init();

  Board_ADC_Init();

  // setup readline/console
  microrl_init(&rl, Board_Print);
  microrl_set_execute_callback(&rl, executerl);
  console_init(&bms_input, &bms_state, &console_output);

  Board_Println("Currently running: "HASH);
  Board_Println("Flashed by: "AUTHOR);

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

    if(Error_Should_Fault()) {
      Board_Println("Requesting Halt..");
      break;
    }
    //TODO: charger enable pin for charger


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
    Board_LTC6804_ProcessInputs(&pack_status, &bms_state);
  }

  // TODO: Reenable after Roman
  // SOC_Estimate(&pack_status);

  // Pack voltage estimation
  // BMS_VOLTAGE_ESTIMATE_T vol = Pack_Estimate_Total_Voltage(&pack_config,
  //                                                          &pack_status);

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
    Error_Present(ERROR_L_CONTACTOR_ERROR);
    if(bms_input->L_contactor_welded) {
      Error_Present(ERROR_L_CONTACTOR_WELD);
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
  pack_config.module_cell_count          = module_cell_count;
  pack_config.cell_min_mV                = CELL_MIN_mV;
  pack_config.cell_max_mV                = CELL_MAX_mV;
  pack_config.cell_capacity_cAh          = CELL_CAPACITY_cAh;
  pack_config.num_modules                = NUM_MODULES;
  pack_config.cell_charge_c_rating_cC    = CELL_CHARGE_C_RATING_cC;
  pack_config.bal_on_thresh_mV           = BALANCE_ON_THRESHOLD_mV;
  pack_config.bal_off_thresh_mV          = BALANCE_OFF_THRESHOLD_mV;
  pack_config.pack_cells_p               = PACK_CELLS_PARALLEL;
  pack_config.cv_min_current_mA          = CV_MIN_CURRENT_mA;
  pack_config.cv_min_current_ms          = CV_MIN_CURRENT_ms;
  pack_config.cc_cell_voltage_mV         = CC_CELL_VOLTAGE_mV;
  pack_config.cell_discharge_c_rating_cC = CELL_DISCHARGE_C_RATING_cC;
  pack_config.max_cell_temp_dC           = MAX_CELL_TEMP_dC;
  pack_config.fan_on_threshold_dC        = FAN_ON_THRESHOLD_dC;
  pack_config.min_cell_temp_dC           = MIN_CELL_TEMP_dC;

  for (size_t i = 0; i < MAX_NUM_MODULES; i++) {
    pack_config.module_cell_count[i] = MODULE_CELL_COUNT;
  }

  bms_input.pack_status      = &pack_status;

  bms_input.H_contactor_welded = false;
  bms_input.L_contactor_welded = false;

  bms_input.H_contactor_closed = false;
  bms_input.L_contactor_closed = false;

  bms_input.ltc_packconfig_check_done = false;
  bms_input.eeprom_read_error         = false;

  bms_output.attempt_ltc_init = true;
  bms_output.balance_req  = balance_reqs;
  memset(balance_reqs,
         0,
         sizeof(balance_reqs[0]) * MAX_NUM_MODULES * MAX_CELLS_PER_MODULE);

  pack_status.cell_voltages_mV     = cell_voltages;
  pack_status.cell_temperatures_dC = cell_temperatures;
  pack_status.pack_cell_max_mV     = 0;
  pack_status.pack_cell_min_mV     = 0xFFFFFFFF;
  pack_status.pack_current_mA      = 0;
  pack_status.pack_voltage_mV      = 0;
  pack_status.max_cell_temp_dC     = 0;

  pack_status.min_cell_temp_dC       = -100;
  pack_status.avg_cell_temp_dC       = 0;
  pack_status.min_cell_temp_position = 0;
  pack_status.max_cell_temp_position = 0;
}
