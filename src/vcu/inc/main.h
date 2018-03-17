#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f2xx_hal.h"
#include "Can_Library.h"

#define CANx                            CAN1
#define CANx_CLK_ENABLE()               __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()
     
#define CANx_FORCE_RESET()              __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()            __HAL_RCC_CAN1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define CANx_TX_PIN                    GPIO_PIN_1
#define CANx_TX_GPIO_PORT              GPIOD  
#define CANx_TX_AF                     GPIO_AF9_CAN1
#define CANx_RX_PIN                    GPIO_PIN_0
#define CANx_RX_GPIO_PORT              GPIOD
#define CANx_RX_AF                     GPIO_AF9_CAN1

/* Definition for USARTx's NVIC */
#define CANx_RX_IRQn                   CAN1_RX0_IRQn
#define CANx_RX_IRQHandler             CAN1_RX0_IRQHandler

void lastRxMsgToFrame(Frame *frame);

#endif /* __MAIN_H */
