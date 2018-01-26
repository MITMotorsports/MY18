#include "main.h"

CAN_HandleTypeDef CanHandle;

/////////////////////////////////////////////

static void CAN_Init();
static void Error_Handler();
static void init();
static void loop();
static void SystemClock_Config();

/////////////////////////////////////////////

static void CAN_Init() {
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
    CanHandle.Init.Prescaler = 4;     // sets the width of the time quanta

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
      
    // Start the reception process and enable reception interrupt
    if (HAL_CAN_Receive_IT(&CanHandle, CAN_FIFO0) != HAL_OK) {
        // Reception Error
        Error_Handler();
    }
}

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

    //BSP_LED_Init(LED1); // LED setup
    //BSP_LED_Init(LED2);
    //BSP_LED_Init(LED3);
    //BSP_LED_Init(LED4);

    //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO); // Button

    CAN_Init(); // CAN
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* CanHandle) {

    uint8_t output = 0;

    if (CanHandle->pRxMsg->IDE == CAN_ID_STD) {
        switch (CanHandle->pRxMsg->StdId) {
            case FrontCanNodeOutput__MSG_ID:
                output = 1;
                break;
            case FrontCanNodeWheelSpeed__MSG_ID:
                output = 2;
                break;
            case BMSCellTemps__MSG_ID:
                output = 3;
                break;
            case BmsPackStatus__MSG_ID:
                output = 4;
                break;
            case BMSErrors__MSG_ID:
                output = 5;
                break;
            case BMS_SOC__MSG_ID:
                output = 6;
                break;
            case Dash_Request__MSG_ID:
                output = 7;
                break;
            case BMSState__MSG_ID:
                output = 8;
                break;
            case VcuToDash__MSG_ID:
                output = 9;
                break;
            case RearCanNodeWheelSpeed__MSG_ID:
                output = 10;
                break;
            case BMS_VCU_Switch__MSG_ID:
                output = 11;
                break;
            case CurrentSensor_Current__MSG_ID:
                output = 12;
                break;
            case CurrentSensor_Voltage__MSG_ID:
                output = 13;
                break;
            case CurrentSensor_Power__MSG_ID:
                output = 14;
                break;
            case MC_Command__MSG_ID:
                output = 15;
                break;
            case BMS_CSB_Switch__MSG_ID:
                output = 16;
                break;
            case BMS_Mode__MSG_ID:
                output = 17;
                break;
            default:
                output = 0;
                break;
        }

        if (output != 0) {
          CanHandle->pTxMsg->StdId = 0x321;
          CanHandle->pTxMsg->RTR = CAN_RTR_DATA;
          CanHandle->pTxMsg->IDE = CAN_ID_STD;
          CanHandle->pTxMsg->DLC = 8;
          CanHandle->pTxMsg->Data[0] = output;

          if (HAL_CAN_Transmit_IT(CanHandle) != HAL_OK) {
            // Error
          }
        }
    } 

    // Receive
    if (HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK) {
        // Reception Error
        Error_Handler();
    }
}

static void loop() {

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
