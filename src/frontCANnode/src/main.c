#include "serial.h"
#include "types.h"
#include "input.h"
#include "state.h"
#include "output.h"
#include "adc.h"

#include "sysinit.h"
#include "chip.h"

#include "Can_Library.h"

#define SERIAL_BAUDRATE 57600
#define CAN_BAUDRATE 500000

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

static ADC_Input_T adc;
static Speed_Input_T speed;
static Can_Input_T can_input;
static Input_T input;

static Rules_State_T rules;
static Torque_State_T torque;
static Can_Output_State_T can_output;
static State_T state;

void SysTick_Handler(void) {
  msTicks++;
}

void initialize_structs(void) {
  input.adc = &adc;
  input.speed = &speed;
  input.can_input = &can_input;

  state.rules = &rules;
  state.torque = &torque;
  state.can_output = &can_output;
}

void fill_input(void) {
  input.msTicks = msTicks;
  Input_fill_input(&input);
}

void update_state(void) {
  State_update_state(&input, &state);
}

void process_output(void) {
  Output_process_output(&input, &state);
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);
  Can_Init(CAN_BAUDRATE);

  ADC_Init();

  initialize_structs();

  Serial_Println("Started up!");

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  while(1) {
    fill_input();
    update_state();
    process_output();
  }
}
