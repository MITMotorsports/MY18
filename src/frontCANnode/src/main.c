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

Input_T input;

void SysTick_Handler(void) {
  msTicks++;
}

void initialize_input() {
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
    speed.disregard[i] = false;
    speed.wheel_stopped[i] = true;

    for (int j = 0; j < NUM_TEETH; j++) {
      speed.last_tick[i][j] = 0;
    }
  }

  speed.last_speed_read_ms = 0;
  speed.front_right_A_wheel_speed = 0;
  speed.front_right_B_wheel_speed = 0;
  speed.front_left_A_wheel_speed = 0;
  speed.front_left_B_wheel_speed = 0;

  input.speed = &speed;
}

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(SERIAL_BAUDRATE);

  init_can0_can_node();

  ADC_Init();

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

    static uint32_t last_print = 0;
    if (false && msTicks - last_print > 10) {
      Serial_Print("brake_1: ");
      Serial_PrintlnNumber(adc.brake_1, 10);

      Serial_Print("brake_2: ");
      Serial_PrintlnNumber(adc.brake_2, 10);

      Serial_Print("accel_1: ");
      Serial_PrintlnNumber(adc.accel_1, 10);

      Serial_Print("accel_2: ");
      Serial_PrintlnNumber(adc.accel_2, 10);

      Serial_Print("accel_1_raw: ");
      Serial_PrintlnNumber(adc.accel_1_raws[ACCEL_LOG_LENGTH-1], 10);

      Serial_Print("accel_2_raw: ");
      Serial_PrintlnNumber(adc.accel_2_raws[ACCEL_LOG_LENGTH-1], 10);

      last_print = msTicks;
    }
  }
}

// Interrupt configs

void handle_interrupt(LPC_TIMER_T* timer, Speed_Input_T *speed, Wheel_T wheel) {
  Chip_TIMER_Reset(timer);            /* Reset the timer immediately */
  Chip_TIMER_ClearCapture(timer, 0);      /* Clear the capture */
  const uint32_t curr_tick = Chip_TIMER_ReadCapture(timer, 0) / CYCLES_PER_MICROSECOND;
  Input_handle_interrupt(msTicks, curr_tick, wheel);
}

// Interrupt handlers. These function get called automatically on
// a rising edge of the signal going into the timer capture pin
void TIMER32_0_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_0, input.speed, LEFT_B);
}

void TIMER32_1_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_1, input.speed, RIGHT_B);
}

void TIMER16_0_IRQHandler(void) {
  handle_interrupt(LPC_TIMER16_0, input.speed, LEFT_A);
}

void TIMER16_1_IRQHandler(void) {
  handle_interrupt(LPC_TIMER16_1, input.speed, RIGHT_A);
}

void Set_Interrupt_Priorities(void) {
  /* Give 32 bit timer capture interrupts the highest priority */
  NVIC_SetPriority(TIMER_32_0_IRQn, 0);
  NVIC_SetPriority(TIMER_16_0_IRQn, 1);
  NVIC_SetPriority(TIMER_32_1_IRQn, 2);
  NVIC_SetPriority(TIMER_16_1_IRQn, 3);
  /* Give the SysTick function a lower priority */
  NVIC_SetPriority(SysTick_IRQn, 4);
}
