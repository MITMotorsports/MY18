
#include "pwm.h"

void Set_Fan_PWM_Duty_Cycle(uint8_t frac) {
	// the duty cycle for the fan is frac / 100 (should be an integer between 1 and 100)
	// Ideally we would know exactly how to set this, but for now we have a linear fit from data
	// Need this because we have to set register for compare not the duty cycle exactly
  // y = 4*frac
	__HAL_TIM_SET_COMPARE(&TimHandle, TIMx_CHANNEL, 4*frac);
}

void PWM_Output_Init() {
	/*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
  TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM3 input clock (TIM3CLK) is set to APB1 clock x 2,
    since APB1 prescaler is equal to 2.
      TIM3CLK = APB1CLK*2
      APB1CLK = HCLK/4
      => TIM3CLK = HCLK/2 = SystemCoreClock/2

    To get TIM3 counter clock at 10 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock / 2) /10 MHz) - 1

    To get TIM3 output clock at 15 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM3 counter clock / TIM3 output clock) - 1
           = 665

    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR + 1)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR + 1)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR + 1)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR + 1)* 100 = 12.5%

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Initialize TIMx peripheral as follows:
       + Prescaler = (SystemCoreClock / 2 / 10000000) - 1
       + Period = (666 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */

  /* Initialize TIMx peripheral as follows:
       + Prescaler = (SystemCoreClock / 2 / 10000000) - 1
       + Period = (666 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */

  // DO NOT DELETE THIS LINE VERY IMPORTANT
  uhPrescalerValue = (uint32_t)(SystemCoreClock / 2 / 10000000) - 1;

  TimHandle.Instance               = TIMx;
  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = PERIOD_VALUE;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    while(1) {}
  }

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 3 */
  sConfig.Pulse = PULSE_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIMx_CHANNEL) != HAL_OK)
  {
    /* Configuration Error */
    while(1) {}
  }

  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 3 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIMx_CHANNEL) != HAL_OK)
  {
    /* PWM generation Error */
    while(1) {}
  }

}


