#include "input.h"
#include "adc.h"

#include <Can_Library.h>

#define ADC_UPDATE_PERIOD_MS 10

void update_adc(Input_T *input);
void update_can(Input_T *input);

void can_process_vcu_dash(Input_T *input);

// TODO Add wheel speed stuff

void Input_initialize(Input_T *input) {
  input->adc->accel_1 = 0;
  input->adc->accel_2 = 0;
  input->adc->brake_1 = 0;
  input->adc->brake_2 = 0;

  input->can_input->limp_state = CAN_VCUTODASH_LIMP_NORMAL; // Based on CAN spec
}

void Input_fill_input(Input_T *input) {
  update_adc(input);
  update_can(input);
}

void update_adc(Input_T *input) {
  ADC_Input_T *adc = input->adc;
  uint32_t next_updated = adc->last_updated + ADC_UPDATE_PERIOD_MS;

  if (next_updated < input->msTicks) {
    adc->accel_1 = ADC_Read(ACCEL_1_CHANNEL);
    adc->accel_2 = ADC_Read(ACCEL_2_CHANNEL);
    adc->brake_1 = ADC_Read(BRAKE_1_CHANNEL);
    adc->brake_2 = ADC_Read(BRAKE_2_CHANNEL);
    adc->last_updated = input->msTicks;
  }
}

void update_can(Input_T *input) {
  Can_MsgID_T msgID = Can_MsgType();
  switch(msgID) {
    case Can_VcuToDash_Msg:
      can_process_vcu_dash(input);
      break;
    default:
      break;
  }
}

void can_process_vcu_dash(Input_T *input) {
  Can_VcuToDash_T msg;
  Can_VcuToDash_Read(&msg);
  input->can_input->limp_state = msg.limp_state;
}
