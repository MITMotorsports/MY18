#ifndef __MAIN_H
#define __MAIN_H

#include <stdbool.h>
#include "stdio.h"

#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "contactors.h"
#include "extern.h"

#ifdef __GNUC__
  /* With GCC Compilers, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

// LEDs --------------------------------------------------------

#define LED_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_PIN                        GPIO_PIN_10
#define LED_PORT                       GPIOC

// MISC GPIOs ---------------------------------------------------

#define L_CONTACTOR_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define L_CONTACTOR_PIN                        GPIO_PIN_7
#define L_CONTACTOR_PORT                       GPIOC

#define H_CONTACTOR_GATE_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define H_CONTACTOR_PIN                  GPIO_PIN_8
#define H_CONTACTOR_PORT                 GPIOC

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


// MACRO DEFINITIONS

#define PIN(name) name ## _PIN
#define PORT(name) name ## _PORT
#define GPIO(name) (PORT(name)), (PIN(name))

#define GPIO_BEGIN_INIT() \
  GPIO_InitTypeDef gpioinit; \
  __HAL_RCC_GPIOA_CLK_ENABLE(); \
  __HAL_RCC_GPIOB_CLK_ENABLE(); \
  __HAL_RCC_GPIOC_CLK_ENABLE();

#define DGPIO_INIT_OUT(name, begin) \
  gpioinit.Pin = PIN(name); \
  gpioinit.Mode = GPIO_MODE_OUTPUT_PP; \
  gpioinit.Pull = GPIO_PULLUP; \
  gpioinit.Speed = GPIO_SPEED_FAST; \
  HAL_GPIO_Init(PORT(name), &gpioinit); \
  HAL_GPIO_WritePin(GPIO(name), begin);

#define DGPIO_INIT_IN(name, type) \
  gpioinit.Pin = PIN(name); \
  gpioinit.Mode = GPIO_MODE_INPUT; \
  gpioinit.Pull = type; \
  gpioinit.Speed = GPIO_SPEED_FAST; \
  HAL_GPIO_Init(PORT(name), &gpioinit);

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
static void SystemClock_Config(void);

USART_HandleTypeDef  USARTHandle;
CAN_HandleTypeDef    CanHandle;

#endif /* __MAIN_H */
