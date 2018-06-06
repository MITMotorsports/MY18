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

void Set_Interrupt_Priorities(void);

static ADC_Errors_T errors;
static ADC_Input_T adc;
static Speed_Input_T speed;

Input_T input;

void SysTick_Handler(void) {
  msTicks++;
}

void initialize_input(void) {
  errors.accel_1_under = false;
  errors.accel_1_over = false;
  errors.accel_2_under = false;
  errors.accel_2_over = false;
  errors.brake_1_under = false;
  errors.brake_1_over = false;
  errors.brake_2_under = false;
  errors.brake_2_over = false;

  adc.errors = &errors;

  // Initialize adc
  adc.accel_1 = 0;
  adc.accel_2 = 0;

  for (int i = 0; i < ACCEL_LOG_LENGTH; i++) {
    adc.accel_1_raws[i] = 0;
    adc.accel_2_raws[i] = 0;
  }

  for (int i = 0; i < BRAKE_LOG_LENGTH; i++) {
    adc.brake_1_raws[i] = 0;
    adc.brake_2_raws[i] = 0;
  }
  adc.brake_1 = 0;
  adc.brake_2 = 0;
  adc.steering_pot = 0;

  input.adc = &adc;

  // Initialize wheel speed sensor
  for (int i = 0; i < NUM_WHEELS; i++) {
    speed.num_ticks[i] = 0;
    speed.big_sum[i] = 0;
    speed.little_sum[i] = 0;
    speed.last_updated[i] = 0;
    speed.wheel_stopped[i] = true;

    for (int j = 0; j < NUM_TEETH; j++) {
      speed.last_tick[i][j] = 0;
    }
  }

  speed.last_speed_read_ms = 0;
  speed.can_node_right_32b_wheel_speed = 0;
  speed.can_node_right_16b_wheel_speed = 0;
  speed.can_node_left_32b_wheel_speed = 0;
  speed.can_node_left_16b_wheel_speed = 0;

  input.speed = &speed;
  input.msTicks = 0;
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);

  init_can0_can_node();

  ADC_Init();

  // Wheel speed timer
  Timer_Init();
  Set_Interrupt_Priorities();
  Timer_Start();

  initialize_input();
  Serial_Println("Started up!");
  Serial_Println("Currently running "HASH);
  Serial_Println("Flashed by: "AUTHOR);

  if (SysTick_Config (SystemCoreClock / 1000)) {
    // Error
    while(1);
  }

  Frame devnull;

  while(1) {
    Can_RawRead(&devnull);

    input.msTicks = msTicks;
    Input_fill_input();
    Output_process_output();
  }
}

// Interrupt configs

void handle_interrupt(LPC_TIMER_T* timer, Wheel_T wheel) {
  // Reset the timer immediately
  Chip_TIMER_Reset(timer);
  // Clear the capture
  Chip_TIMER_ClearCapture(timer, 0);
  const uint32_t curr_tick = Chip_TIMER_ReadCapture(timer, 0) / CYCLES_PER_MICROSECOND;
  Input_handle_interrupt(msTicks, curr_tick, wheel);
}

// Interrupt handlers. These function get called automatically on
// a rising edge or falling edge of the signal going into the timer capture pin
void TIMER32_0_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_0, LEFT_32);
}

void TIMER32_1_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_1, RIGHT_32);
}

void TIMER16_0_IRQHandler(void) {
  handle_interrupt(LPC_TIMER16_0, LEFT_16);
}

void TIMER16_1_IRQHandler(void) {
  handle_interrupt(LPC_TIMER16_1, RIGHT_16);
}

void Set_Interrupt_Priorities(void) {
  // Give 32 bit interrupts the higher priority
  NVIC_SetPriority(TIMER_32_0_IRQn, 0);
  NVIC_SetPriority(TIMER_16_0_IRQn, 1);
  NVIC_SetPriority(TIMER_32_1_IRQn, 0);
  NVIC_SetPriority(TIMER_16_1_IRQn, 1);
  // Give the SysTick function a lower priority
  NVIC_SetPriority(SysTick_IRQn, 2);
}
