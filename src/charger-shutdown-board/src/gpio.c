#include "gpio.h"
#include "uart.h"

// Digital GPIO Initialization
void Board_GPIO_Init(void) {
  Chip_GPIO_Init(LPC_GPIO);

  // LED1
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED1);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED1, IOCON_LED1_FUNC);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED1, false);

  // LED2
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED2);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED2, IOCON_LED2_FUNC);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED2, false);



  // Charge Enable Pin
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_CHARGER_ENABLE);
  Chip_IOCON_PinMuxSet(LPC_IOCON,
                       PIN_IOCON_CHARGER_ENABLE,
                       IOCON_FUNC0 | IOCON_MODE_INACT);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_CHARGER_ENABLE, false);

  //BMS Fault Pin
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_BMS_FAULT);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_BMS_FAULT, IOCON_FUNC0);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_BMS_FAULT, false);

}

// Set the value of a GPIO pin
inline void Board_Pin_Set(uint8_t led_gpio, uint8_t led_pin, bool state) {
  Chip_GPIO_SetPinState(LPC_GPIO, led_gpio, led_pin, state);
}

// Read the value of a GPIO pin
inline bool Board_Pin_Read(uint8_t port, uint8_t pin) {
  return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
}

// Toggle a GPIO pin
inline void Board_Pin_Toggle(uint8_t port, uint8_t pin) {
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, !Board_Pin_Read(port, pin));
}




