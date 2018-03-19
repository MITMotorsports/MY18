#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

#include "chip.h"
#include "pins.h"

// Variables
static ADC_CLOCK_SETUP_T adc_setup;

// Functions
// - Initialize
void Board_GPIO_Init(void);

void Board_ADC_Init(void);

// - Peripherals

// -- LEDs
void Board_LED_On(uint8_t led_gpio,
                  uint8_t led_pin);

void Board_LED_Off(uint8_t led_gpio,
                   uint8_t led_pin);

void Board_LED_Toggle(uint8_t led_gpio,
                      uint8_t led_pin);

// -- Contactors
// --- Set Contactor States
void Board_Contactors_Set(bool close_contactors);

// --- Get Contactor States
bool Board_Contactors_Closed(void);

bool Board_Contactor_High_Closed(void);

bool Board_Contactor_Low_Closed(void);

#endif // ifndef _GPIO_H_
