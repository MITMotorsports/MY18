#include "main.h"


uint32_t heartbeat     = 0;
int16_t  contactor_one = 0;
int16_t  contactor_two = 0;

void Init_BMS_Structs(void) {
  bms_state.pack_config = &pack_config;
  bms_state.curr_mode   = BMS_SSM_MODE_INIT;

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
  pack_config.cell_discharge_c_rating_cC = CELL_DISCHARGE_C_RATING_cC; // at 27
                                                                       // degrees
                                                                       // C
  pack_config.max_cell_temp_dC    = MAX_CELL_TEMP_dC;
  pack_config.fan_on_threshold_dC = FAN_ON_THRESHOLD_dC;
  pack_config.min_cell_temp_dC    = MIN_CELL_TEMP_dC;

  uint8_t i;

  for (i = 0; i < MAX_NUM_MODULES; i++) {
    pack_config.module_cell_count[i] = MODULE_CELL_COUNT;
  }

  bms_input.pack_status      = &pack_status;
  bms_input.mode_request     = BMS_SSM_MODE_STANDBY;
  bms_input.vcu_mode_request = BMS_SSM_MODE_STANDBY;
  bms_input.csb_mode_request = BMS_SSM_MODE_STANDBY;

  bms_input.H_contactor_welded = false;
  bms_input.L_contactor_welded = false;

  bms_input.H_contactor_closed = false;
  bms_input.L_contactor_closed = false;

  bms_input.fault_asserted = false;

  bms_input.ltc_packconfig_check_done = false;
  bms_input.eeprom_read_error         = false;
  bms_input.fan_override              = false;
  bms_input.msTicks                   = msTicks;

  bms_output.assert_fault = false;
  bms_output.balance_req  = balance_reqs;
  memset(balance_reqs,
         0,
         sizeof(balance_reqs[0]) * MAX_NUM_MODULES * MAX_CELLS_PER_MODULE);
  bms_output.ltc_deinit                = false;
  bms_output.check_packconfig_with_ltc = false;

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

void Process_Input(BMS_INPUT_T *bms_input) {
  if (bms_state.curr_mode != BMS_SSM_MODE_INIT) {
    Board_CAN_Receive(bms_input);
    Board_GetModeRequest(&console_output, bms_input);
    Board_LTC6804_ProcessInputs(&pack_status, &bms_state);

    // TODO: Reenable after Roman
    // SOC_Estimate(&pack_status);

    // Pack voltage estimation
    BMS_VOLTAGE_ESTIMATE_T vol = Pack_Estimate_Total_Voltage(&pack_config,
                                                             &pack_status);
  }

  bms_input->msTicks        = msTicks;
  bms_input->fault_asserted = Board_Pin_Read(PIN_BMS_FAULT);

  bms_input->H_contactor_welded = Board_Contactor_High_Welded();
  bms_input->L_contactor_welded = Board_Contactor_Low_Welded();

  bms_input->H_contactor_closed = Board_Contactor_High_Closed();
  bms_input->L_contactor_closed = Board_Contactor_Low_Closed();

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

  // if (bms_input->H_contactor_welded) {
  //   Error_Assert(ERROR_H_CONTACTOR_WELDED);
  // }
  //
  // if (bms_input->L_contactor_welded) {
  //   Error_Assert(ERROR_L_CONTACTOR_WELDED);
  // }
}

void Process_Output(BMS_INPUT_T  *bms_input,
                    BMS_OUTPUT_T *bms_output,
                    BMS_STATE_T  *bms_state) {

  Board_CAN_Transmit(bms_input, bms_output);

  if (bms_output->ltc_deinit) {
    Board_LTC6804_DeInit();
  } else if (bms_output->check_packconfig_with_ltc) {
    bms_input->ltc_packconfig_check_done = Board_LTC6804_Init(&pack_config,
                                                              cell_voltages);
  } else {
    Board_LTC6804_UpdateBalanceStates(bms_output->balance_req);

    // Board_Println("PACKCONFIG CHECK DONE");
  }
}

void Process_Keyboard(void) {
  uint32_t readln = Board_Read(str, 50);
  uint32_t i;

  for (i = 0; i < readln; i++) {
    microrl_insert_char(&rl, str[i]);
  }
}

typedef enum {
  OVERVOLT,
  UNDERVOLT,
  OVERTEMP,
  ERRORS_I_CARE_ABOUT_NUM
} ERRORS_I_CARE_ABOUT_T;

uint32_t times[ERRORS_I_CARE_ABOUT_NUM] = {};
bool Check_Error(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output) {
  if (input->pack_status->pack_cell_min_mV < state->pack_config->cell_min_mV) {
    if (times[UNDERVOLT] == 0) times[UNDERVOLT] = msTicks;
  }
  else {
    times[UNDERVOLT] = 0;
  }

  if ((times[UNDERVOLT] > 0) &&
      (msTicks - times[UNDERVOLT] > CELL_UNDER_VOLTAGE_timeout_ms)) {
    output->assert_fault = false;
    return true;
  }


  if (input->pack_status->pack_cell_max_mV >
      state->pack_config->cell_max_mV + 5) {
    if (times[OVERVOLT] == 0) times[OVERVOLT] = msTicks;
  }
  else {
    times[OVERVOLT] = 0;
  }

  if ((times[OVERVOLT] > 0) &&
      (msTicks - times[OVERVOLT] > CELL_OVER_VOLTAGE_timeout_ms)) {
    output->assert_fault = false;
    return true;
  }

  int16_t max_cell_temp_thres_dC = state->pack_config->max_cell_temp_dC;

  if (input->pack_status->max_cell_temp_dC > max_cell_temp_thres_dC) {
    if (times[OVERTEMP] == 0) times[OVERTEMP] = msTicks;
  }
  else {
    times[OVERTEMP] = 0;
  }

  if ((times[OVERTEMP] > 0) &&
      (msTicks - times[OVERTEMP] > 500)) {
    output->assert_fault = false;
    return true;
  }

  return false;
}

int main(void) {
  Init_BMS_Structs();
  Board_Chip_Init();
  Board_GPIO_Init();
  Board_UART_Init(57600);
  Board_ADC_Init();
  Board_CAN_Init();
  EEPROM_Init(LPC_SSP1, EEPROM_BAUD, EEPROM_CS_PIN);
  SOC_Init(&pack_status);

  Error_Init();
  SSM_Init(&bms_input, &bms_state, &bms_output);

  Board_ADC_Init();

  // setup readline/console
  microrl_init(&rl, Board_Print);
  microrl_set_execute_callback(&rl, executerl);
  console_init(&bms_input, &bms_state, &console_output);
  int32_t count = msTicks;

  Board_Println("Currently running: "HASH);
  Board_Println("Flashed by: "AUTHOR);

  // Check for contactor weld in first 5 seconds
  // while (msTicks - count < 5000) {
  //   Process_Input(&bms_input);
  // }
  Board_Pin_Set(PIN_BMS_FAULT, true);
  Board_Pin_Set(PIN_BMS_FAULT, true);
  Board_Pin_Set(PIN_BMS_FAULT, true);
  Board_Pin_Set(PIN_BMS_FAULT, true);
  while (1) {
    // Setting fault pin high
    Board_Pin_Set(PIN_BMS_FAULT, GPIO_HIGH);

    // Process_Keyboard();                                  // console input
    Process_Input(&bms_input);                           // Processes Inputs(can
                                                         // messages, pin
                                                         // states, cell stats)
    SSM_Step(&bms_input, &bms_state, &bms_output);       // changes state based
                                                         // on inputs
    Process_Output(&bms_input, &bms_output, &bms_state); // Transmits can
                                                         // messages, processes
                                                         // ltc output(update
                                                         // balance states)

    // handler:
    if (Check_Error(&bms_input, &bms_state, &bms_output) ||
        (Error_Handle(bms_input.msTicks) == HANDLER_HALT)) {
      Board_Println("Requesting Halt...");
      break; // Handler requested a Halt
    }
  }
  Board_Pin_Set(PIN_BMS_FAULT, false);
  Board_Pin_Set(PIN_BMS_FAULT, false);
  Board_Println("FORCED HANG");
  Write_EEPROM_Error();

  bms_output.assert_fault = false;

  Board_Println("Halting...");

  while (1) {
    Board_Pin_Set(PIN_BMS_FAULT, false);
    bms_input.msTicks = msTicks;
    Process_Output(&bms_input, &bms_output, &bms_state);
    // Process_Keyboard();
    bms_state.curr_mode = BMS_SSM_MODE_STANDBY;
  }

  return 0;
}
