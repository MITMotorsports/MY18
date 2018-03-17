#include "led.h"
#include "pins.h"
#include "board.h"

// Depends on whether LPC is being used to sink current
// or switching fets.
#define _LED_ON 1
#define _LED_OFF 0

void LED_HV_on() {
    Pin_Write(PIN_LED_HV, _LED_ON);
}

void LED_HV_off() {
    Pin_Write(PIN_LED_HV, _LED_OFF);
}

void LED_RTD_on() {
    Pin_Write(PIN_LED_RTD, _LED_ON);
}

void LED_RTD_off() {
    Pin_Write(PIN_LED_RTD, _LED_OFF);
}

void LED_IMD_on() {
    Pin_Write(PIN_LED_IMD, _LED_ON);
}

void LED_IMD_off() {
    Pin_Write(PIN_LED_IMD, _LED_OFF);
}

void LED_AMS_on() {
    Pin_Write(PIN_LED_AMS, _LED_ON);
}

void LED_AMS_off() {
    Pin_Write(PIN_LED_AMS, _LED_OFF);
}
