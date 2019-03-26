#include "main.h"
#include "fault_gates.h"
#include "can_handles.h"

int main(void) {
  HAL_Init();

  SystemClock_Config();

  Can_Init(500000);
  USART_Init(115200);

  GPIO_BEGIN_INIT();

  PWM_Output_Init(); // FAN PWM AND TIMERS

  /// INPUTS
  DGPIO_INIT_IN(       SDN, GPIO_NOPULL);
  DGPIO_INIT_IN(  SDN_GATE, GPIO_NOPULL);
  DGPIO_INIT_IN(  BMS_GATE, GPIO_NOPULL);
  DGPIO_INIT_IN(  IMD_GATE, GPIO_NOPULL);
  DGPIO_INIT_IN(  BPD_GATE, GPIO_NOPULL);

  DGPIO_INIT_IN(  FAKE_IMD, GPIO_NOPULL);

  DGPIO_INIT_IN(MASTER_RST, GPIO_PULLUP);

  // Low Side Contactor closed signal from BMS
  DGPIO_INIT_IN(L_CONTACTOR_STATUS, GPIO_NOPULL);

  /// OUTPUTS
  // HVDCDC Disable Pin:
  //   - When it is HIGH HVDCDC is disabled
  //   - Normally LOW, except during precharge
  DGPIO_INIT_OUT(     HVDCDC, GPIO_PIN_RESET);

  // Battery Fans
  // Actually high all the time
  DGPIO_INIT_OUT(       BFAN, GPIO_PIN_SET);

  // Setup an LED for debugging
  DGPIO_INIT_OUT(        LED, GPIO_PIN_RESET);

  // Brake Light
  DGPIO_INIT_OUT(BRAKE_LIGHT, GPIO_PIN_RESET);

  // Brake Valve
  DGPIO_INIT_OUT(BRAKE_VALVE, GPIO_PIN_RESET);

  // Driver Reset GPIO output for when Driver Reset is pressed
  DGPIO_INIT_OUT( DRIVER_RST, GPIO_PIN_SET);

  DGPIO_INIT_OUT(ACTIVE_AERO, GPIO_PIN_RESET);

  // SETUP THE CONTACTOR GPIOS
  DGPIO_INIT_OUT(L_CONTACTOR, GPIO_PIN_RESET);
  DGPIO_INIT_OUT(H_CONTACTOR, GPIO_PIN_RESET);

  // BEGIN
  printf(ASCII_ART);
  printf("Currently running: "HASH"\r\n");
  printf("Flashed by: "AUTHOR"\r\n");

  printf("\r\n\r\n");

  init_states();

  while (1) {
    advance_states();
    send_VCU();

    // TODO: Find a better location for this.
    // Update brake light all the time.
    HAL_GPIO_WritePin(GPIO(BRAKE_LIGHT),
                      pedalbox.brake_2 > PEDALBOX_BRAKE_BEGIN);

    HAL_GPIO_WritePin(GPIO(ACTIVE_AERO), control_settings.active_aero_enabled);

    static uint32_t lastt = 0;

    print_gate_faults(false);
    if (HAL_GetTick() - lastt > 100) {
      HAL_GPIO_TogglePin(GPIO(LED));

      if (mc_readings.can_fault) {
        send_mc_fault_clear();
      }

      // printf("CONTROLS PARAMS:\r\n  using_regen: %d\r\n  using_voltage_limiting: %d\r\n  using_temp_limiting: %d\r\n  regen_bias: %d\r\n  limp_factor: %d\r\n  temp_lim_min_gain: %d\r\n  temp_lim_thresh_temp: %d\r\n  temp_abs_thesh: %d\r\n  volt_lim_min_gain: %d\r\n  volt_lim_min_voltage: %d\r\n  volt_abs_thresh: %d\r\n\r\n",
      // control_settings.using_regen, control_settings.using_voltage_limiting, control_settings.using_temp_limiting, control_settings.regen_bias, control_settings.limp_factor, control_settings.temp_lim_min_gain, control_settings.temp_lim_thresh_temp, MAX_TEMP, control_settings.volt_lim_min_gain, control_settings.volt_lim_min_voltage, MIN_VOLTAGE);


      lastt = HAL_GetTick();
    }
  }
}

void Error_Handler(const char *s) {
  while (1) {
    printf("FORCED HANG IN Error_Handler\r\n");
    printf(           "Error Message: %s\r\n", s);
    HAL_Delay(1000);
  }
}

// THE NETHER REGIONS -- TREAD CAREFULLY

// FOR REFERENCE:
// typedef enum
// {
//   HAL_OK       = 0x00U,
//   HAL_ERROR    = 0x01U,
//   HAL_BUSY     = 0x02U,
//   HAL_TIMEOUT  = 0x03U
// } HAL_StatusTypeDef;

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE {
  /* Place your implementation of fputc here */

  /* e.g. write a character to the EVAL_COM1 and Loop until the end of
     transmission */
  HAL_USART_Transmit(&USARTHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void USART_Init(uint32_t baudrate) {
  // Setup USART for debugging
  USARTHandle.Instance         = USARTx_INSTANCE;
  USARTHandle.Init.BaudRate    = baudrate;
  USARTHandle.Init.WordLength  = USART_WORDLENGTH_8B;
  USARTHandle.Init.StopBits    = USART_STOPBITS_1;
  USARTHandle.Init.Parity      = USART_PARITY_NONE;
  USARTHandle.Init.Mode        = USART_MODE_TX_RX;
  USARTHandle.Init.CLKPolarity = USART_POLARITY_LOW;
  USARTHandle.Init.CLKPhase    = USART_PHASE_1EDGE;
  USARTHandle.Init.CLKLastBit  = USART_LASTBIT_DISABLE;

  if (HAL_USART_Init(&USARTHandle) != HAL_OK) {
    Error_Handler("UART Initialization");
  }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 120000000
 *            HCLK(Hz)                       = 120000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 240
 *            PLL_P                          = 2
 *            PLL_Q                          = 5
 *            VDD(V)                         = 3.3
 *            Flash Latency(WS)              = 3
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM       = 24;
  RCC_OscInitStruct.PLL.PLLN       = 240;
  RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ       = 5;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler("Oscillator Initialization");
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType =
    (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
     RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
    Error_Handler("Clock Config Initialization");
  }
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  printf("Assertion failed on: file %s on line %d. HALTING...\r\n", file, line);

  while (1) {}
}

#endif /* ifdef  USE_FULL_ASSERT */
