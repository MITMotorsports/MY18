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

  // LED3
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED3);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED3, IOCON_LED3_FUNC);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED3, false);

  // SSP for EEPROM
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC2 | IOCON_MODE_INACT)); //
                                                                                   // MISO1
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC2 | IOCON_MODE_INACT)); //
                                                                                   // MOSI1
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC2 | IOCON_MODE_INACT)); //
                                                                                   // SCK1

  // SSP for LTC6804
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8,
                       (IOCON_FUNC1 | IOCON_MODE_PULLUP));                         // MISO0
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9,
                       (IOCON_FUNC1 | IOCON_MODE_PULLUP));                         // MOSI0
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC2 | IOCON_MODE_INACT)); // SCK0
  Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO0_6);

  // TODO: is the cs pin supposed to be initialized?
  // Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 |
  // IOCON_MODE_INACT));

  // FSAE specific pin intializations

  // Contactor Weld
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, PIN_CONTACTOR_WELD_1);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CONTACTOR_WELD_1,
                       (IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_INACT));


  Chip_GPIO_SetPinDIRInput(LPC_GPIO, PIN_CONTACTOR_WELD_2);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CONTACTOR_WELD_2,
                       (IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_INACT));

  // for analog mode
  // Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CONTACTOR_WELD_2,
  //                      (IOCON_FUNC2 | IOCON_ADMODE_EN));

  // High Side Detect
  // No internal pull up/down. Exists in hardware.
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, PIN_HIGH_SIDE_DETECT);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_HIGH_SIDE_DETECT,
                       (IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_INACT));

  // Low Side Detect
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, PIN_LOW_SIDE_DETECT);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LOW_SIDE_DETECT,
                       (IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_INACT));

  // This is the analog config that does not work. TODO: Firgure out why.
  // Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LOW_SIDE_DETECT,
  // (IOCON_FUNC0 | IOCON_ADMODE_EN | IOCON_MODE_INACT));

  // Charge Enable Pin
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_CHARGER_ENABLE);
  Chip_IOCON_PinMuxSet(LPC_IOCON,
                       PIN_IOCON_CHARGER_ENABLE,
                       IOCON_FUNC0 | IOCON_MODE_INACT);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_CHARGER_ENABLE, false);

  // Fault Pin
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_BMS_FAULT);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_BMS_FAULT, IOCON_FUNC0);
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_BMS_FAULT, false);

  // Enable pull down resistors on unused pins
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_23);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_23,
                       (IOCON_PIN_23_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_23, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_29);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_29,
                       (IOCON_PIN_29_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_29, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_32);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_32,
                       (IOCON_PIN_32_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_32, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_33);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_33,
                       (IOCON_PIN_33_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_33, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_9);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_9,
                       (IOCON_PIN_9_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_9, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_17);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_17,
                       (IOCON_PIN_17_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_17, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_2);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_2,
                       (IOCON_PIN_2_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_2, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_21);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_21,
                       (IOCON_PIN_21_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_21, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_1);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_1,
                       (IOCON_PIN_1_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_1, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_11);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_11,
                       (IOCON_PIN_11_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_11, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_12);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_12,
                       (IOCON_PIN_12_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_12, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_25);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_25,
                       (IOCON_PIN_25_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_25, false);

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_48);
  Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_48,
                       (IOCON_PIN_48_FUNC | IOCON_MODE_PULLDOWN));
  Chip_GPIO_SetPinState(LPC_GPIO, PIN_48, false);
}

// Set the value of a GPIO pin
inline void Board_Pin_Set(uint8_t led_gpio, uint8_t led_pin, bool state) {
  Chip_GPIO_SetPinState(LPC_GPIO, led_gpio, led_pin, state);
}

// Read the value of a GPIO pin
inline bool Board_Pin_Read(uint8_t gpio, uint8_t pin) {
  return Chip_GPIO_GetPinState(LPC_GPIO, gpio, pin);
}

// Toggle a GPIO pin
inline void Board_Pin_Toggle(uint8_t gpio, uint8_t pin) {
  Chip_GPIO_SetPinState(LPC_GPIO, gpio, pin, !Board_Pin_Read(gpio, pin));
}

// Read from an ADC channel

// Analog Pin Initialization
void Board_ADC_Init() {
  Chip_ADC_Init(LPC_ADC, &adc_setup);
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH4, ENABLE);
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH5, ENABLE);
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH7, ENABLE);
  Chip_ADC_SetBurstCmd(LPC_ADC, 1);
  Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}

bool Board_Contactor_Low_Closed() {
  return Board_Pin_Read(PIN_LOW_SIDE_DETECT);
}

bool Board_Contactor_High_Closed() {
  return Board_Pin_Read(PIN_HIGH_SIDE_DETECT);
}

bool Board_Contactor_Low_Welded() {
  return !Board_Pin_Read(PIN_CONTACTOR_WELD_2);
}

bool Board_Contactor_High_Welded() {
  return !Board_Pin_Read(PIN_CONTACTOR_WELD_1);
}
