#ifndef __MAIN_H
#define __MAIN_H

#include <stdbool.h>
#include "stdio.h"

#include "stm32f2xx_hal.h"
#include "CANlib.h"

#include "extern.h"
#include "gpio.h"
#include "state.h"
#include "pwm.h"

#ifdef __GNUC__

// With GCC Compilers, small printf
// (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar()

  # define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else // ifdef __GNUC__
  # define PUTCHAR_PROTOTYPE int fputc(int ch, FILE * f)
#endif /* __GNUC__ */

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
// MOVED TO can_handles.h

// PWM -----------------------------------------------------------
// MOVED TO pwm.h

void USART_Init(uint32_t baudrate);
static void SystemClock_Config(void);

USART_HandleTypeDef USARTHandle;
CAN_HandleTypeDef   CanHandle;

#define ASCII_ART "                              _.-='_-         _\r\n \
                         _.-='   _-          | ||'''''''---._______     __..\r\n \
             ___.===''''-.______-,,,,,,,,,,,,`-''----' '''''       '''''  __'\r\n \
      __.--''     __        ,'                   o \\           __        [__|\r\n \
 __-''=======.--''  ''--.=================================.--''  ''--.=======:\r\n \
]       [w] : /        \\ : |========================|    : /        \\ :  [w] :\r\n \
V___________:|          |: |========================|    :|          |:   _-'\r\n \
 V__________: \\        / :_|=======================/_____: \\        / :__-'\r\n \
 -----------'  '-____-'  `-------------------------------'  '-____-'\r\n"


#endif // ifndef __MAIN_H
