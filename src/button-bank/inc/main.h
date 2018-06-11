#ifndef __MAIN_H
#define __MAIN_H

#include <string.h>

#include "chip.h"
#include "sysinit.h"
#include "CANlib.h"

#include "uart.h"

extern volatile uint32_t msTicks;

/// UX TIMING CONFIGURATIONS
#define LED_BLINK_PERIOD 500
#define BUZZ_DURATION 2000

#define DEBOUNCE_SETUP 50
#define DEBOUNCE_HOLD  100

#define VCU_INTRO_TIME 100

/// GPIO CONFIGS

// INPUTS
#ifdef _BUTTONBANK_V2_
#define BTN_DRIVER_RST 1, 2
#define BTN_DRIVER_RST_IOCON IOCON_PIO1_2

#define DRIVER_RST_LED 1, 0
#define DRIVER_RST_LED_IOCON IOCON_PIO1_0

#define BTN_RTD        1, 1
#define BTN_RTD_IOCON  IOCON_PIO1_1

#define RTD_LED 3, 0
#define RTD_LED_IOCON IOCON_PIO3_0

#define BTN_A 3, 1
#define BTN_A_IOCON IOCON_PIO3_1

#define BTN_A_LED 2, 3
#define BTN_A_LED_IOCON IOCON_PIO2_3

#define BTN_B 1, 4
#define BTN_B_IOCON IOCON_PIO1_4

#define BTN_B_LED 1, 3
#define BTN_B_LED_IOCON IOCON_PIO1_3

#else
#define BTN_DRIVER_RST 1, 1
#define BTN_DRIVER_RST_IOCON IOCON_PIO1_1

#define BTN_SCROLL_SEL 1, 2
#define BTN_SCROLL_SEL_IOCON IOCON_PIO1_4

#define BTN_RTD        1, 4
#define BTN_RTD_IOCON  IOCON_PIO1_2
#endif

#define BTN_CONFIG IOCON_FUNC0 | IOCON_DIGMODE_EN
#define BTN_DOWN false // Map GPIO to button state. (pull resistor dependent)
#define LED_ON true
#define LED_OFF false


// OUTPUTS
#define BUZZER 1, 5
#define BUZZER_IOCON IOCON_PIO1_5
#define BUZZER_CONFIG IOCON_FUNC0

/// GPIO MACROS
#define READ_PIN(name) Chip_GPIO_GetPinState(LPC_GPIO, name)
#define SET_PIN(name, val) Chip_GPIO_SetPinState(LPC_GPIO, name, val)

// #define TOGGLE_PIN(name) SET_PIN(name, !READ_PIN(name))

/// BUTTON STATE STRUCTURE
typedef enum {
  rtd,
  driver_reset,
  A,
  B,
  LEN_BUTTONS  // Last element in an enum cast to int gives #(elements defined)
} BUTTONS;

extern bool button_state[LEN_BUTTONS];

typedef struct {
  can0_VCUHeartbeat_vcu_state_T vcu_state;
  can0_VCUHeartbeat_error_state_T error_state;
} car_states_t;


/// FUNCTION DEFINITIONS
void Serial_Init(uint32_t baudrate);

// GPIO FUNCTIONS
void GPIO_Init(void);

void poll_buttons(bool* button_state);
void print_buttons(bool* bs);

void buzz(void);
void button_leds(void);


#define CAN_Init() init_can0_button_bank()

// CAN FUNCTIONS
void can_error_handler(Can_ErrorID_T error);
void can_read(void);
bool send_buttonrequest(bool* bs);

#endif // ifndef __MAIN_H
