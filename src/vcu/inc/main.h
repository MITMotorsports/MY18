#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "contactors.h"

// LEDs --------------------------------------------------------

#define LED_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_PIN                        GPIO_PIN_10
#define LED_PORT                       GPIOC

// MISC GPIOs ---------------------------------------------------

#define VCU_GATE_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define VCU_GATE_PIN                        GPIO_PIN_7
#define VCU_GATE_PORT                       GPIOC

#define PRECHARGE_GATE_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define PRECHARGE_GATE_PIN                  GPIO_PIN_8
#define PRECHARGE_GATE_PORT                 GPIOC

#define FAULT_GATES_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define FAULT_GATES_PORT                    GPIOC
#define BMS_FAULT_GATE_PIN                  GPIO_PIN_4
#define BPD_FAULT_GATE_PIN                  GPIO_PIN_6
#define IMD_FAULT_GATE_PIN                  GPIO_PIN_5
#define SDN_FAULT_GATE_PIN                  GPIO_PIN_2
#define ESD_FAULT_PIN                       GPIO_PIN_3

#define DRIVER_RESET_TRIGGER_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define DRIVER_RESET_TRIGGER_PIN            GPIO_PIN_1
#define DRIVER_RESET_TRIGGER_PORT           GPIOC


// USART -------------------------------------------------------
#define USARTx_INSTANCE               USART1
#define USARTx_CLK_ENABLE() __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define USARTx_FORCE_RESET() __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET() __HAL_RCC_USART1_RELEASE_RESET()

#define USARTx_TX_PIN                 GPIO_PIN_6
#define USARTx_TX_GPIO_PORT           GPIOB
#define USARTx_TX_AF                  GPIO_AF7_USART1
#define USARTx_RX_PIN                 GPIO_PIN_7
#define USARTx_RX_GPIO_PORT           GPIOB
#define USARTx_RX_AF                  GPIO_AF7_USART1

#define USARTx_RX_IRQn                USART1_IRQn
#define USARTx_RX_IRQHandler          USART1_IRQHandler

// CAN ------------------------------------------------------------
#define CANx                            CAN1
#define CANx_CLK_ENABLE() __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define CANx_FORCE_RESET() __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET() __HAL_RCC_CAN1_RELEASE_RESET()

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
