#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

#include "chip.h"
#include "pins.h"

// GPIO digital representation
#define GPIO_HIGH true
#define GPIO_LOW false

// Variables
static ADC_CLOCK_SETUP_T adc_setup;

// Functions
// - Initialize
void Board_GPIO_Init(void);

void Board_ADC_Init(void);

// - Peripherals

// -- Digital Pins
void Board_Pin_Set(uint8_t led_gpio, uint8_t led_pin, bool state);
bool Board_Pin_Read(uint8_t gpio, uint8_t pin);
void Board_Pin_Toggle(uint8_t gpio, uint8_t pin);


// -- Contactors
// --- Get Contactor States

bool Board_Contactor_High_Closed(void);
bool Board_Contactor_Low_Closed(void);

bool Board_Contactor_High_Welded(void);
bool Board_Contactor_Low_Welded(void);

#endif // ifndef _GPIO_H_
