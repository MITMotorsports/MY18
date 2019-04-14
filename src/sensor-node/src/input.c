#include "input.h"

extern Input_T input;
extern volatile uint32_t msTicks;

void update_adc(void);
void update_wheel_speed(void);
void update_can(void);


uint16_t transform1(uint32_t accel_1_raw);
uint16_t transform2(uint32_t accel_2_raw);

void Input_fill_input() {
  update_adc();
}

void update_adc() {
  ADC_Input_T *adc = input.adc;
  uint32_t next_updated = adc->last_updated + ADC_UPDATE_PERIOD_MS;


  if (next_updated < input.msTicks) {
    adc->steering_pot = ADC_Read(STEERING_CHANNEL);
    adc->last_updated = input.msTicks;
  }
}
