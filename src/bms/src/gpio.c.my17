#include "gpio.h"

// Digital GPIO Initialization
void Board_GPIO_Init(void) {
    Chip_GPIO_Init(LPC_GPIO);
    // LED1
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_LED1, IOCON_LED1_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, LED1, false);

    // LED2
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED2);
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_LED2, IOCON_LED2_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, LED2, false);

    //SSP for EEPROM
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* MISO1 */ 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* MOSI1 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* SCK1 */

    //SSP for LTC6804
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC1 | IOCON_MODE_PULLUP));   /* MISO0 */ 
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC1 | IOCON_MODE_PULLUP));   /* MOSI0 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* SCK0 */

        // High Side Contactor Pin
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, FSAE_CTR_HIGH_SWTCH);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_CTR_HIGH_SWTCH, (IOCON_FUNC0 | IOCON_MODE_INACT));

    // Fault Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, FSAE_FAULT_GPIO);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_FAULT_GPIO,
            (IOCON_FUNC0 | IOCON_MODE_INACT));

    // Fault starts in off state (so shutdown loop is open)
    Chip_GPIO_SetPinState(LPC_GPIO, FSAE_FAULT_GPIO, false);

    // Charge Enable Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, FSAE_CHARGE_ENABLE_GPIO);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_CHARGE_ENABLE_GPIO,
            (IOCON_FUNC0 | IOCON_MODE_INACT));

    // Charge Enable starts in off state
    Chip_GPIO_SetPinState(LPC_GPIO, FSAE_CHARGE_ENABLE_GPIO, false);

    // DC DC Enable Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, FSAE_DC_DC_ENABLE_GPIO);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_DC_DC_ENABLE_GPIO,
            (IOCON_FUNC0 | IOCON_MODE_INACT));

    // DC DC Enable starts in off state
    Chip_GPIO_SetPinState(LPC_GPIO, FSAE_DC_DC_ENABLE_GPIO, false);

    // DC DC Fault Pin
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, FSAE_DC_DC_FAULT_GPIO);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_DC_DC_FAULT_GPIO,
            (IOCON_FUNC0 | IOCON_MODE_INACT));

    // Fan pin config
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, FSAE_FAN_1_PIN);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_FAN_1_PIN,
            (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_OPENDRAIN_EN));

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, FSAE_FAN_2_PIN);
    Chip_IOCON_PinMuxSet(LPC_IOCON, FSAE_IOCON_FAN_2_PIN,
            (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_OPENDRAIN_EN));

    Chip_GPIO_SetPinState(LPC_GPIO, FSAE_FAN_1_PIN, false);
    Chip_GPIO_SetPinState(LPC_GPIO, FSAE_FAN_2_PIN, false);

    // Enable pull down resistors on unused pins
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_37);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_37_IOCON, 
        (PIN_37_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_37, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_35);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_35_IOCON, 
        (PIN_35_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_35, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_34);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_34_IOCON, 
        (PIN_34_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_34, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_33);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_33_IOCON, 
        (PIN_33_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_33, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_32);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_32_IOCON, 
        (PIN_32_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_32, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_31);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_31_IOCON, 
        (PIN_31_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_31, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_30);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_30_IOCON, 
        (PIN_30_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_30, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_29);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_29_IOCON, 
        (PIN_29_PIO_FUNC | IOCON_MODE_PULLDOWN) );
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_29, false);



















}


void Board_LED_On(uint8_t led_gpio, uint8_t led_pin) {
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, led_gpio, led_pin);
}

void Board_LED_Off(uint8_t led_gpio, uint8_t led_pin) {
    Chip_GPIO_SetPinOutLow(LPC_GPIO, led_gpio, led_pin);
}

void Board_LED_Toggle(uint8_t led_gpio, uint8_t led_pin) {
    Chip_GPIO_SetPinState(LPC_GPIO, led_gpio, led_pin,
        1 - Chip_GPIO_GetPinState(LPC_GPIO, led_gpio, led_pin));
}

void Board_Contactors_Set(bool close_contactors) {
    Chip_GPIO_SetPinState(LPC_GPIO, FSAE_FAULT_GPIO, close_contactors);
}

bool Board_Contactors_Closed(void) {
    return Chip_GPIO_GetPinState(LPC_GPIO, FSAE_FAULT_GPIO);
}

// Analog Pin Initialization
void Board_ADC_Init() {
  Chip_ADC_Init(LPC_ADC, &adc_setup);
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH4, ENABLE);
  Chip_ADC_SetBurstCmd(LPC_ADC, 1);
  Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}

// bool Board_Contactor_Two_Welded() {
//   int16_t adc_data;

//   while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH4, ADC_DR_DONE_STAT)) {}
//   Chip_ADC_ReadValue(LPC_ADC, ADC_CH4, &adc_data);
//   return adc_data < 800;
// }

// // Digital
// bool Board_Contactor_One_Welded(void) {
//   // returns False when shorted

//   return Chip_GPIO_GetPinState(LPC_GPIO, PIN_CONTACTOR_WELD_1);
// }
