#define _CANMGR_H__
#ifdef _CANMGR_H__

#include "CANlib.h"
#include "data.h"
#include "speed.h"

extern volatile uint32_t msTicks;

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (msTicks - last_sent < period) return;                 \
  last_sent = msTicks;

void can_transmit(uint16_t* ext_adc_data, uint16_t* int_adc_data, Speed_Input_T* speed_val);

void can_transmit_adc_data1(uint16_t* ext_adc_data);
void can_transmit_adc_data2(uint16_t* ext_adc_data);
void can_transmit_adc_data3(uint16_t* int_adc_data);

void can_transmit_wheel_speed(Speed_Input_T* speed_val);

void handle_can_error(Can_ErrorID_T error);

#endif