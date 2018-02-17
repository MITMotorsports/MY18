#ifndef _PINS_H
#define _PINS_H

#define PIN_OLED_RS     3, 1
#define PIN_OLED_CS     3, 3

#define PIN_OLED_SCLK   2, 4
#define PIN_OLED_SDI    2, 5
#define PIN_OLED_SDO    2, 6 

#define PIN_OLED_BS0    2, 0
#define PIN_OLED_BS1    2, 1
#define PIN_OLED_BS2    2, 2

#define PIN_LED_HV      2, 3
#define PIN_LED_RTD     1, 11
#define PIN_LED_IMD     1, 8
#define PIN_LED_AMS     1, 9

#define PIN_BUTTON1     1, 3
#define PIN_BUTTON2     1, 5 

#define IOCON_BUTTON1 IOCON_PIO1_3
#define IOCON_BUTTON2 IOCON_PIO1_5

#endif
