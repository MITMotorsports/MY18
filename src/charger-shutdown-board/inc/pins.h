#ifndef _PINS_H_
#define _PINS_H_

#define PIN_BMS_FAULT           2, 8
#define PIN_CHARGER_ENABLE      1, 8
#define PIN_IMD_IN 		2, 6
#define PIN_INTERLOCK		0, 2
#define PIN_PRECHARGE 		2, 1
#define PIN_CONTACTORS_CLOSED   0, 3

#define PIN_LED1                2, 3
#define PIN_LED2                1, 11

#define PIN_IOCON_BMS_FAULT           IOCON_PIO2_8
#define PIN_IOCON_CHARGER_ENABLE      IOCON_PIO1_8
#define PIN_IOCON_IMD_IN	      IOCON_PIO2_6
#define PIN_IOCON_INTERLOCK	      IOCON_PIO0_2
#define PIN_IOCON_PRECHARGE	      IOCON_PIO2_1
#define PIN_IOCON_CONTACTORS_CLOSED   IOCON_PIO0_3
#define PIN_IOCON_LED1                IOCON_PIO2_3
#define PIN_IOCON_LED2                IOCON_PIO1_11

#define IOCON_LED1_FUNC IOCON_FUNC0
#define IOCON_LED2_FUNC IOCON_FUNC0


#endif // ifndef _PINS_H_
