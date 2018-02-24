#include "serial.h"
#include "types.h"
#include "input.h"
#include "output.h"
#include "adc.h"

#include "timer.h"

#include "sysinit.h"
#include "chip.h"

#include "CANlib.h"

#define SERIAL_BAUDRATE 57600
#define CAN_BAUDRATE 500000

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

static ADC_Input_T adc;
static Speed_Input_T speed;
static Input_T input;

static Output_T output;

void SysTick_Handler(void) {
  msTicks++;
}

void handle_interrupt(LPC_TIMER_T* timer, Speed_Input_T *speed, Wheel_T wheel) {
  Chip_TIMER_Reset(timer);            /* Reset the timer immediately */
  Chip_TIMER_ClearCapture(timer, 0);      /* Clear the capture */
  const uint32_t curr_tick = Chip_TIMER_ReadCapture(timer, 0) / CYCLES_PER_MICROSECOND;
  Input_handle_interrupt(speed, msTicks, curr_tick, wheel);
}

// Interrupt handler for timer 0 capture pin. This function get called automatically on
// a rising edge of the signal going into the timer capture pin
void TIMER32_0_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_0, input.speed, LEFT);
}

// Interrupt handler for timer 1 capture pin. This function get called automatically on
// a rising edge of the signal going into the timer capture pin
void TIMER32_1_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_1, input.speed, RIGHT);
}

void Set_Interrupt_Priorities(void) {
  /* Give 32 bit timer capture interrupts the highest priority */
  NVIC_SetPriority(TIMER_32_0_IRQn, 0);
  NVIC_SetPriority(TIMER_32_1_IRQn, 1);
  /* Give the SysTick function a lower priority */
  NVIC_SetPriority(SysTick_IRQn, 2);
}

void initialize_structs(void) {
  input.adc = &adc;
  input.speed = &speed;
}

void fill_input(void) {
  input.msTicks = msTicks;
  Input_fill_input(&input);
}

void process_output(void) {
  Output_process_output(&input, &output);
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);
  init_can0_can_node();

  ADC_Init();

  initialize_structs();

  Serial_Println("Started up!");

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  while(1) {
    fill_input();
    process_output();
  }
}
