#include "main.h"

//#define DEBUG_UART true

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

// ADC Data Containers
uint16_t ext_adc_data[8];
uint16_t int_adc_data[4];

void SysTick_Handler(void) {
  msTicks++;
}

Speed_Input_T speed_val;

#define ADC_UPDATE_PERIOD 100
#define WHEEL_SPEED_TIMEOUT_MS 100
#define WHEEL_SPEED_READ_PERIOD_MS 10

int main(void) {
    SystemCoreClockUpdate();
    if (SysTick_Config (SystemCoreClock / 1000)) while(1);  // Hang on error

    Board_UART_Init(57600);

    GPIO_Init();

    Init_SPI_ADC();
    Init_Internal_ADC();

    Timer_Init();
    Set_Interrupt_Priorities();
    Timer_Start();

    init_can0_sensor_node();


    uint32_t adc_update = 0;

	while (1) {
#ifdef DEBUG_UART
        //Board_Print("Speed: ");
        //Board_PrintNum(speed_val.can_node_left_32b_wheel_speed, 10);
        //Board_Print(" / ");
        //Board_PrintNum(speed_val.can_node_left_32b_wheel_speed, 10);
        //Board_Println(""); 
#endif

        if (speed_val.last_speed_read_ms + WHEEL_SPEED_READ_PERIOD_MS < msTicks) {
            update_wheel_speed();
        }


#if DEBUG_UART
        Board_Print_BLOCKING("CH2: ");
        Board_PrintNum(Read_Internal_ADC(ADC_CH2), 10);
        Board_Println_BLOCKING("");
        Board_Print_BLOCKING("CH3: ");
        Board_PrintNum(Read_Internal_ADC(ADC_CH3), 10);
        Board_Println_BLOCKING("");
        Board_Print_BLOCKING("CH4: ");
        Board_PrintNum(Read_Internal_ADC(ADC_CH4), 10);
        Board_Println_BLOCKING("");
        Board_Print_BLOCKING("CH5: ");
        Board_PrintNum(Read_Internal_ADC(ADC_CH5), 10);
        Board_Println_BLOCKING("");
#endif

        Read_Internal_ADC_Range(int_adc_data, 0, 4, 1);
        can_transmit(ext_adc_data, int_adc_data, &speed_val);
        
        adc_update = msTicks + ADC_UPDATE_PERIOD;
	}

	return 0;
}

// wheel speed code very sloppily copied from can node



void Input_handle_interrupt(uint32_t msTicks, uint32_t curr_tick, Wheel_T wheel);

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
  //Board_Println("left wheel?");
}

void TIMER32_1_IRQHandler(void) {
  handle_interrupt(LPC_TIMER32_1, RIGHT_32);
  //Board_Println("right wheel?");
}

void Set_Interrupt_Priorities(void) {
  // Give 32 bit interrupts the higher priority
  NVIC_SetPriority(TIMER_32_0_IRQn, 0);
  NVIC_SetPriority(TIMER_32_1_IRQn, 0);
  // Give the SysTick function a lower priority
  NVIC_SetPriority(SysTick_IRQn, 2);
}

void Input_handle_interrupt(uint32_t msTicks, uint32_t curr_tick, Wheel_T wheel) {
  Speed_Input_T *speed = &speed_val;
  if (false && speed->wheel_stopped[wheel]) {
    speed->num_ticks[wheel] = 0;
    speed->big_sum[wheel] = 0;
    speed->little_sum[wheel] = 0;
    speed->last_updated[wheel] = msTicks;
    return;
  }

  const uint32_t count = speed->num_ticks[wheel];
  const uint8_t idx = count % NUM_TEETH;
  const uint32_t this_tooth_last_rev =
    count < NUM_TEETH ? 0 : speed->last_tick[wheel][idx];

  // Register tick
  speed->last_tick[wheel][idx] = curr_tick;
  speed->num_ticks[wheel]++;

  // Update big sum
  speed->big_sum[wheel] += NUM_TEETH * curr_tick;
  speed->big_sum[wheel] -= speed->little_sum[wheel];

  // Update little sum
  speed->little_sum[wheel] += curr_tick;
  speed->little_sum[wheel] -= this_tooth_last_rev;

  // Update timestamp
  speed->last_updated[wheel] = msTicks;
}
//
// Microsecond = 1 millionth of a second
#define MICROSECONDS_PER_SECOND_F 1000000.0
// 1000 millirevs = 1 rev
#define MILLIREVS_PER_REV_F 1000.0
#define SECONDS_PER_MINUTE 60

uint32_t click_time_to_mRPM(uint32_t us_per_click) {
  if (us_per_click == 0) {
    return 0;
  }
  // Convert milliseconds per click to milli rpm
  const float us_per_rev = us_per_click * 1.0 * NUM_TEETH * 2;

  const float s_per_rev = us_per_rev / MICROSECONDS_PER_SECOND_F;

  const float mrev_per_s = MILLIREVS_PER_REV_F / s_per_rev;

  const float mrev_per_min = mrev_per_s * SECONDS_PER_MINUTE;
  return mrev_per_min;
}

void update_wheel_speed() {
  Speed_Input_T* speed = &speed_val;
  if (speed->last_speed_read_ms + WHEEL_SPEED_READ_PERIOD_MS < msTicks) {
    // Capture values
    speed->last_speed_read_ms = msTicks;
    uint8_t wheel;
    for(wheel = 0; wheel < NUM_WHEELS; wheel++) {
      const uint32_t count = speed->num_ticks[wheel];
      uint8_t idx;
      if (count > 0) {
        // If there are x ticks so far, the last tick is index (x - 1)
        idx = (count - 1) % NUM_TEETH;
      } else {
        idx = 0;
      }

      uint32_t moving_avg;
      if (count < NUM_TEETH) {
        moving_avg = 0;
      } else {
        const uint32_t avg = speed->big_sum[wheel] / SUM_ALL_TEETH;
        moving_avg = avg;
      }
      const bool timeout =
        speed->last_updated[wheel] + WHEEL_SPEED_TIMEOUT_MS < msTicks;

      uint32_t calculated_speed;
      speed->wheel_stopped[wheel] = timeout;
      if (speed->wheel_stopped[wheel]) {
        calculated_speed = 0;
      } else {
        if (count < NUM_TEETH) {
          calculated_speed = click_time_to_mRPM(speed->last_tick[wheel][idx]);
        } else {
          calculated_speed = click_time_to_mRPM(moving_avg);
        }
      }

      // 32 bit timer speeds are first in the enum, so it's safe to look back at
      // their values
      switch (wheel) {
        case LEFT_32:
          speed->can_node_left_32b_wheel_speed = calculated_speed;
          break;
        case RIGHT_32:
          speed->can_node_right_32b_wheel_speed = calculated_speed;
          break;
        default:
          continue;
      }
    }
  }
}


