#ifndef __MAIN_H
#define __MAIN_H

#include <string.h>

#include "sysinit.h"
#include "chip.h"

#include "CANlib.h"

extern volatile uint32_t msTicks;

// GPIO CONFIGS
#define DRIVER_RST 1, 1
#define SCROLL_SEL 1, 2
#define RTD        1, 4

#define DRIVER_RST_IOCON IOCON_PIO1_1
#define SCROLL_SEL_IOCON IOCON_PIO1_4
#define RTD_IOCON        IOCON_PIO1_2

#define BTN_CONFIG IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_PULLDOWN
#define BTN_DOWN true // Map GPIO to button state. (pull resistor dependent)

#define READ_PIN(name) Chip_GPIO_GetPinState(LPC_GPIO, name)

typedef struct {
  bool rtd;
  bool driver_reset;
} button_states_t;


void Serial_Init(uint32_t baudrate);
void GPIO_Init(void);

void print(const char *str);

button_states_t poll_buttons(void);
void print_buttons(button_states_t bs);


#define CAN_Init() init_can0_button_bank()

bool send_buttonrequest(button_states_t hold);
void handle_can_error(Can_ErrorID_T error);

#endif // ifndef __MAIN_H
