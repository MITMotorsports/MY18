#include "stm32f2xx_hal.h"

// LEDs --------------------------------------------------------

#define LED_PIN                        GPIO_PIN_10
#define LED_PORT                       GPIOC

// MISC GPIOs ---------------------------------------------------

#define MASTER_RST_PIN                GPIO_PIN_0
#define MASTER_RST_PORT               GPIOC

#define DRIVER_RST_PIN                GPIO_PIN_1
#define DRIVER_RST_PORT               GPIOC


#define L_CONTACTOR_PIN               GPIO_PIN_7
#define L_CONTACTOR_PORT              GPIOC

#define H_CONTACTOR_PIN               GPIO_PIN_8
#define H_CONTACTOR_PORT              GPIOC


#define SDN_PIN                       GPIO_PIN_2
#define SDN_PORT                      GPIOC

#define SDN_GATE_PIN                  GPIO_PIN_3
#define SDN_GATE_PORT                 GPIOC

#define BMS_GATE_PIN                  GPIO_PIN_4
#define BMS_GATE_PORT                 GPIOC

#define IMD_GATE_PIN                  GPIO_PIN_5
#define IMD_GATE_PORT                 GPIOC

#define BPD_GATE_PIN                  GPIO_PIN_6
#define BPD_GATE_PORT                 GPIOC


// MACRO DEFINITIONS

#define PIN(name) name ## _PIN
#define PORT(name) name ## _PORT
#define GPIO(name) (PORT(name)), (PIN(name))
#define READ_PIN(name) (HAL_GPIO_ReadPin(GPIO(name)) == GPIO_PIN_SET)

#define GPIO_BEGIN_INIT()       \
  GPIO_InitTypeDef gpioinit;    \
  __HAL_RCC_GPIOA_CLK_ENABLE(); \
  __HAL_RCC_GPIOB_CLK_ENABLE(); \
  __HAL_RCC_GPIOC_CLK_ENABLE();

#define DGPIO_INIT_OUT(name, begin)     \
  gpioinit.Pin   = PIN(name);           \
  gpioinit.Mode  = GPIO_MODE_OUTPUT_PP; \
  gpioinit.Pull  = GPIO_PULLUP;         \
  gpioinit.Speed = GPIO_SPEED_FAST;     \
  HAL_GPIO_Init(PORT(name), &gpioinit); \
  HAL_GPIO_WritePin(GPIO(name), begin);

#define DGPIO_INIT_IN(name, type)   \
  gpioinit.Pin   = PIN(name);       \
  gpioinit.Mode  = GPIO_MODE_INPUT; \
  gpioinit.Pull  = type;            \
  gpioinit.Speed = GPIO_SPEED_FAST; \
  HAL_GPIO_Init(PORT(name), &gpioinit);

// Input types:
//  - GPIO_NOPULL
//  - GPIO_PULLUP
//  - GPIO_PULLDOWN
