#ifndef __MAIN_H
#define __MAIN_H

#include <string.h>

#include "chip.h"
#include "sysinit.h"
#include "CANlib.h"

#include "uart.h"

extern volatile uint32_t msTicks;

#define BUZZ_DURATION 2000

/// GPIO CONFIGS

// INPUTS
#define DRIVER_RST 1, 1
#define DRIVER_RST_IOCON IOCON_PIO1_1

#define SCROLL_SEL 1, 2
#define SCROLL_SEL_IOCON IOCON_PIO1_4

#define RTD        1, 4
#define RTD_IOCON  IOCON_PIO1_2

#define BTN_CONFIG IOCON_FUNC0 | IOCON_DIGMODE_EN
#define BTN_DOWN true // Map GPIO to button state. (pull resistor dependent)


// OUTPUTS
#define BUZZER 1, 5
#define BUZZER_IOCON IOCON_PIO1_5
#define BUZZER_CONFIG IOCON_FUNC0

/// GPIO MACROS
#define READ_PIN(name) Chip_GPIO_GetPinState(LPC_GPIO, name)
#define SET_PIN(name, val) Chip_GPIO_SetPinState(LPC_GPIO, name, val)

// #define TOGGLE_PIN(name) SET_PIN(name, !READ_PIN(name))

/// BUTTON STATE STRUCT
typedef struct {
  bool rtd;
  bool driver_reset;
  bool scroll_select;
} button_states_t;

typedef struct {
  can0_VCUHeartbeat_vcu_state_T vcu_state;
  can0_VCUHeartbeat_error_state_T error_state;
} car_states_t;


/// FUNCTION DEFINITIONS

// INITIALIZATIONS
void Serial_Init(uint32_t baudrate);
void GPIO_Init(void);

// GPIO FUNCTIONS
button_states_t poll_buttons(void);
void            print_buttons(button_states_t bs);


#define CAN_Init() init_can0_button_bank()

// CAN FUNCTIONS
bool send_buttonrequest(button_states_t hold);
void can_error_handler(Can_ErrorID_T error);

#endif // ifndef __MAIN_H
