
#ifndef __PWM_H
#define __PWM_H

#include "stm32f2xx_hal.h"

// - TIMERS FOR FAN PWM -------------------------------------------
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#define TIMx                           TIM2
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM2_CLK_ENABLE()

/* Definition for TIMx Channel Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOA_CLK_ENABLE();
#define TIMx_GPIO_PORT_CHANNEL3        GPIOA
#define TIMx_GPIO_PIN_CHANNEL3         GPIO_PIN_2
#define TIMx_GPIO_AF_CHANNEL3          GPIO_AF1_TIM2
#define TIMx_CHANNEL                   TIM_CHANNEL_3

// PWM Settings
#define  PERIOD_VALUE       (uint32_t)(400 - 1)  // Period Value: Experimentally got about 25.5 kHZ with this
#define  PULSE_VALUE       (uint32_t)(PERIOD_VALUE*50/100) // Duty cycle: 50% (to start off with)

TIM_HandleTypeDef TimHandle; // Timer handler typdef
TIM_OC_InitTypeDef sConfig; // timer output compare configuration structure decl
static uint32_t uhPrescalerValue = 0; // counter prescaler value

void PWM_Output_Init();
void Set_Fan_PWM_Duty_Cycle(uint8_t frac);

#endif // ifndef __PWM_H


