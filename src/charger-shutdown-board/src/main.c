#include "main.h"

int main(void) {
  Board_Chip_Init();
  Board_GPIO_Init();
  Board_UART_Init(57600);
  //Board_CAN_Init();

  Error_Init();

  Board_ADC_Init();


  while (1) {
    // Setting fault pin high

    Process_Keyboard();
    Process_Input(&bms_input, &bms_output);
    Process_Output(&bms_input, &bms_output, &bms_state);

    //TODO: charger enable pin for charger


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

  if(bms_input->H_contactor_welded != bms_input->H_contactor_closed) {
    Error_Present(ERROR_H_CONTACTOR_ERROR);
    if(bms_input->H_contactor_welded) {
      Error_Present(ERROR_H_CONTACTOR_WELD);
    }
  } else {
    Error_Clear(ERROR_H_CONTACTOR_ERROR);
    Error_Clear(ERROR_H_CONTACTOR_WELD);
  }

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


