#include "board.h"

#include "cell_temperatures.h"
#include "error_handler.h"

#define TEST_HARDWARE

const uint32_t OscRateIn = 0;

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
