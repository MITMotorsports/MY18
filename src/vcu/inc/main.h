#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f2xx_hal.h"
#include "CANlib.h"

// LEDs --------------------------------------------------------

#define LED_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_PIN					   	   GPIO_PIN_10
#define LED_PORT					   GPIOC

// USART -------------------------------------------------------
#define USARTx_INSTANCE               USART1
#define USARTx_CLK_ENABLE()			  __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOB_CLK_ENABLE()

#define USARTx_TX_PIN                 GPIO_PIN_6
#define USARTx_TX_GPIO_PORT           GPIOB
#define USARTx_TX_AF                  GPIO_AF7_USART1
#define USARTx_RX_PIN                 GPIO_PIN_7
#define USARTx_RX_GPIO_PORT           GPIOB
#define USARTx_RX_AF                  GPIO_AF7_USART1

// CAN ------------------------------------------------------------
#define CANx                            CAN1
#define CANx_CLK_ENABLE()               __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
     
#define CANx_FORCE_RESET()              __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()            __HAL_RCC_CAN1_RELEASE_RESET()

#define CANx_TX_PIN                    GPIO_PIN_9
#define CANx_TX_GPIO_PORT              GPIOB
#define CANx_TX_AF                     GPIO_AF9_CAN1
#define CANx_RX_PIN                    GPIO_PIN_8
#define CANx_RX_GPIO_PORT              GPIOB
#define CANx_RX_AF                     GPIO_AF9_CAN1

#define CANx_RX_IRQn                   CAN1_RX0_IRQn
#define CANx_RX_IRQHandler             CAN1_RX0_IRQHandler

void lastRxMsgToFrame(Frame *frame);

#endif /* __MAIN_H */
