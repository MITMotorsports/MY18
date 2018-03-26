#include "stm32f2xx_hal.h"

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
