#include "main.h"
#include <stdbool.h>

CAN_HandleTypeDef CanHandle;

/////////////////////////////////////////////

static void Error_Handler();
static void init();
static void loop();
static void SystemClock_Config();

/////////////////////////////////////////////

static void Error_Handler() {
    while (1) {

    }
}

static void SystemClock_Config(){

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

static void init() {
    HAL_Init(); // high level libraries

    SystemClock_Config(); // System Clock to 120 MHz

    Can_Init(500000); // CAN
}

static void loop() {
    Can_MsgID_T msgID = Can_MsgType();

    uint8_t output = 0;

    switch(msgID) {
      case Can_FrontCanNodeOutput_Msg:
        output = 1;
        break;
      default:
        output = 0;
        break;
    }

    if (output == 1) {

      Can_MC_Command_T msg;
      msg.torque = 10000;
      msg.speed = 0;
      msg.direction_is_counterclockwise = 0;
      msg.inverter_enabled = 1;
      msg.discharge_enabled = 0;
      msg.torque_limit = 0;

      Can_MC_Command_Write(&msg);
    }
}

int main() {
    init();

    while(1) {
        loop();
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
