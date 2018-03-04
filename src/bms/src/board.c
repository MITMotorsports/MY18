#include "board.h"

#include "cell_temperatures.h"
#include "error_handler.h"

#define TEST_HARDWARE

const uint32_t OscRateIn = 0;

static LTC6804_CONFIG_T ltc6804_config;
static LTC6804_STATE_T  ltc6804_state;
static Chip_SSP_DATA_SETUP_T ltc6804_xf_setup;
static uint8_t  ltc6804_tx_buf[LTC6804_CALC_BUFFER_LEN(MAX_NUM_MODULES)];
static uint8_t  ltc6804_rx_buf[LTC6804_CALC_BUFFER_LEN(MAX_NUM_MODULES)];
static uint8_t  ltc6804_cfg[LTC6804_DATA_LEN];
static uint16_t ltc6804_bal_list[MAX_NUM_MODULES];
static LTC6804_ADC_RES_T ltc6804_adc_res;
static LTC6804_OWT_RES_T ltc6804_owt_res;

// ltc6804 timing variables
static bool _ltc6804_gcv;
static uint32_t _ltc6804_last_gcv;
static uint32_t _ltc6804_gcv_tick_time;
static bool     _ltc6804_owt;
static uint32_t _ltc6804_last_owt;
static uint32_t _ltc6804_owt_tick_time;
static uint32_t board_lastThermistorShiftTime_ms = 0;

uint8_t currentThermistor                     = 0;
static bool ltc6804_setMultiplexerAddressFlag = false;
static bool ltc6804_getThermistorVoltagesFlag = false;

static bool _ltc6804_initialized;
static LTC6804_INIT_STATE_T _ltc6804_init_state;

// msTicks increment interrupt handler
void SysTick_Handler(void) {
  msTicks++;
}

void Board_Chip_Init(void) {
  SysTick_Config(Hertz2Ticks(1000));
}

void Board_BlockingDelay(uint32_t delayms) {
  uint32_t haltTime = msTicks + delayms;

  while (msTicks < haltTime) ;
}

bool Board_LTC6804_CVST() {
#ifdef TEST_HARDWARE_LTC_TEST
  return true;

#else /* ifdef TEST_HARDWARE_LTC_TEST */
  LTC6804_STATUS_T res;
  uint32_t count = msTicks;

  // while(msTicks - count < 2000) {
  //     Board_PrintNum(msTicks,10);
  // }
  res = LTC6804_CVST(&ltc6804_config, &ltc6804_state, msTicks);

  switch (res) {
  case LTC6804_FAIL:
    Board_Println("CVST FAIL");
    Error_Assert(ERROR_LTC6804_CVST, msTicks);
    return false;

  case LTC6804_PEC_ERROR:
    Board_Println("CVST PEC_ERROR");
    Error_Assert(ERROR_LTC6804_PEC, msTicks);
    return false;

  case LTC6804_PASS:
    Board_Println("CVST PASS");
    Error_Pass(ERROR_LTC6804_CVST);
    return true;

  case LTC6804_WAITING:
  case LTC6804_WAITING_REFUP:
    return false;

  default:
    Board_Println("Unhandled case in Board_LTC6804_CVST.");
    return false;
  }

  return false;

#endif /* ifdef TEST_HARDWARE_LTC_TEST */
}

void Board_LTC6804_UpdateBalanceStates(bool *balance_req) {
#ifdef TEST_HARDWARE
  UNUSED(balance_req);
  return;

#else /* ifdef TEST_HARDWARE */
  LTC6804_UpdateBalanceStates(&ltc6804_config,
                              &ltc6804_state,
                              balance_req,
                              msTicks);
#endif /* ifdef TEST_HARDWARE */
}

bool Board_LTC6804_OpenWireTest(void) {
#ifdef TEST_HARDWARE_LTC_TEST
  return true; // Change to simulate during test

#else  /* ifdef TEST_HARDWARE_LTC_TEST */

  if (msTicks - _ltc6804_last_owt > _ltc6804_owt_tick_time) {
    _ltc6804_owt = true;
  }

  if (!_ltc6804_owt) {
    return false;
  }

  LTC6804_STATUS_T res;
  res = LTC6804_OpenWireTest(&ltc6804_config,
                             &ltc6804_state,
                             &ltc6804_owt_res,
                             msTicks);

  switch (res) {
  case LTC6804_FAIL:
    Board_Print("OWT FAIL, mod=");
    itoa(ltc6804_owt_res.failed_module, str, 10);
    Board_Print(str);
    Board_Print(" wire=");
    itoa(ltc6804_owt_res.failed_wire, str, 10);
    Board_Println(str);
    Error_Assert(ERROR_LTC6804_OWT, msTicks);
    return false;

  case LTC6804_PEC_ERROR:
    Board_Println("OWT PEC_ERROR");
    Error_Assert(ERROR_LTC6804_PEC, msTicks);
    return false;

  case LTC6804_PASS:
    Board_Println("OWT PASS");
    _ltc6804_owt      = false;
    _ltc6804_last_owt = msTicks;
    Error_Pass(ERROR_LTC6804_OWT);
    return true;

  case LTC6804_WAITING:
  case LTC6804_WAITING_REFUP:

    return false;

  default:
    Board_Println("Unhandled case in Board_LTC6804_OpenWireTest.");
    return false;
  }
#endif /* ifdef TEST_HARDWARE_LTC_TEST */
}

bool Board_LTC6804_Init(BMS_PACK_CONFIG_T *pack_config,
                        uint32_t          *cell_voltages_mV) {
#ifdef TEST_HARDWARE_LTC_TEST
  UNUSED(pack_config); UNUSED(cell_voltages_mV);
  return true;

#else /* ifdef TEST_HARDWARE_LTC_TEST */

  if (_ltc6804_initialized) return true;

  bool res;

  switch (_ltc6804_init_state) {
  case LTC6804_INIT_NONE:
    ltc6804_config.pSSP    = LPC_SSP0;
    ltc6804_config.baud    = LTC6804_BAUD;
    ltc6804_config.cs_gpio = 0;
    ltc6804_config.cs_pin  = 2;

    ltc6804_config.num_modules       = pack_config->num_modules;
    ltc6804_config.module_cell_count = pack_config->module_cell_count;

    ltc6804_config.min_cell_mV = pack_config->cell_min_mV;
    ltc6804_config.max_cell_mV = pack_config->cell_max_mV;

    ltc6804_config.adc_mode = LTC6804_ADC_MODE_NORMAL;

    ltc6804_state.xf       = &ltc6804_xf_setup;
    ltc6804_state.tx_buf   = ltc6804_tx_buf;
    ltc6804_state.rx_buf   = ltc6804_rx_buf;
    ltc6804_state.cfg      = ltc6804_cfg;
    ltc6804_state.bal_list = ltc6804_bal_list;

    ltc6804_adc_res.cell_voltages_mV = cell_voltages_mV;

    ltc6804_owt_res.failed_wire   = 0;
    ltc6804_owt_res.failed_module = 0;

    _ltc6804_gcv           = false;
    _ltc6804_last_gcv      = 0;
    _ltc6804_gcv_tick_time = 100;
    _ltc6804_owt           = true;
    _ltc6804_last_owt      = 0;
    _ltc6804_owt_tick_time = 60000;


    LTC6804_Init(&ltc6804_config, &ltc6804_state, msTicks);

    _ltc6804_init_state = LTC6804_INIT_CFG;
    break;

  case LTC6804_INIT_CFG:
    res = Board_LTC6804_CVST();

    if (res) {
      // Board_Println("CVST passed");
      _ltc6804_init_state = LTC6804_INIT_CVST;
    }
    break;

  case LTC6804_INIT_CVST:

    // Board_Println("CVST state");
    res = Board_LTC6804_OpenWireTest();

    if (res) {
      //  Board_Println("OWT passed");
      _ltc6804_init_state = LTC6804_INIT_DONE;
    }
    break;

  case LTC6804_INIT_DONE:

    // Board_Println("ltc init done");
    _ltc6804_initialized = true;
    _ltc6804_init_state  = 0;
    return true;
  }
  return false;

#endif /* ifdef TEST_HARDWARE_LTC_TEST */
}

void Board_LTC6804_DeInit(void) {
#ifndef TEST_HARDWARE
  _ltc6804_initialized = false;
  _ltc6804_init_state  = LTC6804_INIT_NONE;
#endif /* ifndef TEST_HARDWARE */
}

void Board_GetModeRequest(const CONSOLE_OUTPUT_T *console_output,
                          BMS_INPUT_T            *bms_input) {
  // create console mode request
  BMS_SSM_MODE_T console_mode_request = BMS_SSM_MODE_STANDBY;

  // then get console mode request
  if (console_output->valid_mode_request) {
    console_mode_request = console_output->mode_request;
  }

  if ((console_mode_request == BMS_SSM_MODE_STANDBY) &&
      (bms_input->csb_mode_request == BMS_SSM_MODE_STANDBY)) {
    bms_input->mode_request = bms_input->vcu_mode_request;
  }

  // if console mode request is standby vcu mode request is standby:
  // mode request is charger mode request
  // Error pass conflicting mode requests
  // else if (charge mode request is standby and vcu mode requst is stadndby):
  // mode request is console mode request
  // Error pass conflicting mode requests
  // else if console mode request is standby and charge mode reuqest is standby
  // mode request is vcu mode request
  // else if console mode request == discharge mode request or cosole mode
  // request == charge mode request
  // mode request is console
}

void Board_LTC6804_ProcessInputs(BMS_PACK_STATUS_T *pack_status,
                                 BMS_STATE_T       *bms_state) {
  Board_LTC6804_GetCellVoltages(pack_status);
  Board_LTC6804_GetCellTemperatures(pack_status,
                                    bms_state->pack_config->num_modules);
  Board_LTC6804_OpenWireTest();
}

void Board_LTC6804_GetCellVoltages(BMS_PACK_STATUS_T *pack_status) {
#ifdef TEST_HARDWARE_LTC_TEST
  UNUSED(pack_status);
  return;

#else /* ifdef TEST_HARDWARE_LTC_TEST */

  if (msTicks - _ltc6804_last_gcv > _ltc6804_gcv_tick_time) {
    _ltc6804_gcv = true;
  }

  if (!_ltc6804_gcv) {
    return;
  }

  LTC6804_STATUS_T res = LTC6804_GetCellVoltages(&ltc6804_config,
                                                 &ltc6804_state,
                                                 &ltc6804_adc_res,
                                                 msTicks);

  switch (res) {
  case LTC6804_FAIL:
    Board_Println("Get Vol FAIL");
    break;

  case LTC6804_PEC_ERROR:
    Board_Println("Get Vol PEC_ERROR");
    Error_Assert(ERROR_LTC6804_PEC, msTicks);
    break;

  case LTC6804_PASS:
    Board_Println("Get Vol Success");
    pack_status->pack_cell_min_mV = ltc6804_adc_res.pack_cell_min_mV;
    pack_status->pack_cell_max_mV = ltc6804_adc_res.pack_cell_max_mV;
    Board_PrintNum(pack_status->pack_cell_max_mV, 10);
    Board_PrintNum(pack_status->pack_cell_min_mV, 10);
    LTC6804_ClearCellVoltages(&ltc6804_config, &ltc6804_state, msTicks); // [TODO]
                                                                         // Use
                                                                         // this
                                                                         // to
                                                                         // your
                                                                         // advantage
    _ltc6804_gcv      = false;
    _ltc6804_last_gcv = msTicks;
    Error_Pass(ERROR_LTC6804_PEC);

  case LTC6804_WAITING:
  case LTC6804_WAITING_REFUP:
    break;

  default:
    Board_Println("WTF");
  }
#endif /* ifdef TEST_HARDWARE_LTC_TEST */
}

void Board_LTC6804_GetCellTemperatures(BMS_PACK_STATUS_T *pack_status,
                                       uint8_t            num_modules) {
#ifndef TEST_HARDWARE_LTC_TEST

  if ((msTicks - board_lastThermistorShiftTime_ms) > TIME_PER_THERMISTOR_MS) {
    board_lastThermistorShiftTime_ms = msTicks;

    // if we finished reading previous thermistor voltage, go to next thermistor
    if (!ltc6804_setMultiplexerAddressFlag &&
        !ltc6804_getThermistorVoltagesFlag) {
      if (currentThermistor < (MAX_THERMISTORS_PER_MODULE - 1)) {
        currentThermistor += 1;
      } else {
        currentThermistor = 0;
      }

      // set flags to true
      ltc6804_setMultiplexerAddressFlag = true;
      ltc6804_getThermistorVoltagesFlag = true;
    }
  }
  LTC6804_STATUS_T status;

  // set multiplexer address
  // if flag is not true, skip this step
  if (ltc6804_setMultiplexerAddressFlag) {
    // initalize CLOCK and LATCH input to the shift register
    status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                                  LTC6804_SHIFT_REGISTER_CLOCK, 0, msTicks);
    Board_HandleLtc6804Status(status);

    if (status != LTC6804_PASS) return;

    status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                                  LTC6804_SHIFT_REGISTER_LATCH, 0, msTicks);
    Board_HandleLtc6804Status(status);

    if (status != LTC6804_PASS) return;

    // Get thermistor address
    uint8_t thermistorAddress = 0;

    if (currentThermistor <= THERMISTOR_GROUP_ONE_END) {
      thermistorAddress = currentThermistor + THERMISTOR_GROUP_ONE_OFFSET;
    } else if ((THERMISTOR_GROUP_TWO_START <= currentThermistor) &&
               (currentThermistor <= THERMISTOR_GROUP_TWO_END)) {
      thermistorAddress = currentThermistor + THERMISTOR_GROUP_TWO_OFFSET;
    } else if ((THERMISTOR_GROUP_THREE_START <= currentThermistor) &&
               (currentThermistor <= THERMISTOR_GROUP_THREE_END)) {
      thermistorAddress = currentThermistor + THERMISTOR_GROUP_THREE_OFFSET;
    } else {
      Board_Println(
        "Invalid value of currentThermistor Board_LTC6804_GetCellTemperatures");
      Error_Assert(ERROR_CONTROL_FLOW, msTicks);
    }

    // shift bits into shift resgister
    int8_t i;

    for (i = 7; i >= 0; i--) {
      uint8_t addressBit = (thermistorAddress & (1 << i)) >> i;
      status = LTC6804_SetGPIOState(&ltc6804_config,
                                    &ltc6804_state,
                                    LTC6804_SHIFT_REGISTER_DATA_IN,
                                    addressBit,
                                    msTicks);
      Board_HandleLtc6804Status(status);

      if (status != LTC6804_PASS) return;

      status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                                    LTC6804_SHIFT_REGISTER_CLOCK, 1, msTicks);
      Board_HandleLtc6804Status(status);

      if (status != LTC6804_PASS) return;

      status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                                    LTC6804_SHIFT_REGISTER_CLOCK, 0, msTicks);
      Board_HandleLtc6804Status(status);

      if (status != LTC6804_PASS) return;
    }

    // Latch the outputs
    status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                                  LTC6804_SHIFT_REGISTER_LATCH, 1, msTicks);
    Board_HandleLtc6804Status(status);

    if (status != LTC6804_PASS) return;

    status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                                  LTC6804_SHIFT_REGISTER_LATCH, 0, msTicks);
    Board_HandleLtc6804Status(status);

    if (status != LTC6804_PASS) return;

    // Finished setting multiplexer address. Reset flag
    ltc6804_setMultiplexerAddressFlag = false;
  }

  // Get thermistor voltages
  // if flag is not true, return
  if (!ltc6804_getThermistorVoltagesFlag) {
    return;
  }

  uint32_t gpioVoltages[MAX_NUM_MODULES * LTC6804_GPIO_COUNT];
  status = LTC6804_GetGPIOVoltages(&ltc6804_config, &ltc6804_state, gpioVoltages,
                                   msTicks);
  Board_HandleLtc6804Status(status);

  if (status != LTC6804_PASS) return;

  CellTemperatures_UpdateCellTemperaturesArray(gpioVoltages, currentThermistor,
                                               pack_status, num_modules);

  // Finished getting thermistor voltages. Reset flag
  ltc6804_getThermistorVoltagesFlag = false;

  if (currentThermistor == THERMISTOR_GROUP_THREE_END) {
    CellTemperatures_UpdateMaxMinAvgCellTemperatures(pack_status, num_modules);
  }


#else /* ifndef TEST_HARDWARE_LTC_TEST */
  UNUSED(pack_status);
  UNUSED(num_modules);
#endif // TEST_HARDWARE
}

#ifndef TEST_HARDWARE_LTC_TEST
void Board_HandleLtc6804Status(LTC6804_STATUS_T status) {
  switch (status) {
  case LTC6804_WAITING:
    break;

  case LTC6804_PASS:
    Error_Pass(ERROR_LTC6804_PEC);
    break;

  case LTC6804_FAIL:
    Board_Println("LTC6804 fail");
    break;

  case LTC6804_PEC_ERROR:
    Board_Println("LTC6804 PEC_ERROR");
    Error_Assert(ERROR_LTC6804_PEC, msTicks);
    break;

  case LTC6804_WAITING_REFUP:
    break;

  default:
    Board_Println("Unhandled case in Board_HandleLtc6804Status.");
    Error_Assert(ERROR_CONTROL_FLOW, msTicks);
  }
}

#endif // TEST_HARDWARE
