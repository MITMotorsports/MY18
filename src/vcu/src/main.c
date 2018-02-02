
#include "main.h"

#include <stdbool.h>

CAN_HandleTypeDef    CanHandle;
bool shouldSend = false;

static void CAN_Config(void);
static void SystemClock_Config(void);
static void Error_Handler(void);

int main(void)
{
  HAL_Init();
  
  SystemClock_Config();
    
  CAN_Config();
  
  if(HAL_CAN_Receive_IT(&CanHandle, CAN_FIFO0) != HAL_OK)
  {
    Error_Handler();
  }
    
  while(1)
  {
    if (shouldSend) {
      /* Set the data to be transmitted */
      CanHandle.pTxMsg->StdId = 0x321;
      CanHandle.pTxMsg->RTR = CAN_RTR_DATA;
      CanHandle.pTxMsg->IDE = CAN_ID_STD;
      CanHandle.pTxMsg->DLC = 2;
      CanHandle.pTxMsg->Data[0] = 0x01;
      CanHandle.pTxMsg->Data[1] = 0xAD;
      /*##-3- Start the Transmission process ###############################*/
      if(HAL_CAN_Transmit(&CanHandle, 10) != HAL_OK)
      {
        /* Transmission Error */
        Error_Handler();
      }
      HAL_Delay(10);
      shouldSend = false;
    }
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
static void SystemClock_Config(void)
{
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

static void Error_Handler(void)
{
  while(1)
  {
  }
}

static void CAN_Config(void)
{
  CAN_FilterConfTypeDef  sFilterConfig;
  static CanTxMsgTypeDef        TxMessage;
  static CanRxMsgTypeDef        RxMessage;
  
  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CAN1;
  CanHandle.pTxMsg = &TxMessage;
  CanHandle.pRxMsg = &RxMessage;
    
  CanHandle.Init.TTCM = DISABLE;
  CanHandle.Init.ABOM = DISABLE;
  CanHandle.Init.AWUM = DISABLE;
  CanHandle.Init.NART = DISABLE;
  CanHandle.Init.RFLM = DISABLE;
  CanHandle.Init.TXFP = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_NORMAL;
  CanHandle.Init.SJW = CAN_SJW_1TQ;
  CanHandle.Init.BS1 = CAN_BS1_6TQ;
  CanHandle.Init.BS2 = CAN_BS2_8TQ;
  CanHandle.Init.Prescaler = 4;
  
  if(HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterNumber = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
  
  if(HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }
      
  /*##-3- Configure Transmission process #####################################*/
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* CanHandle)
{
  shouldSend = true;
  
  /* Receive */
  if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
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

