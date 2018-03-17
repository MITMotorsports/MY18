#ifndef _PINS_H_
#define _PINS_H_

#define LED1 2, 10
#define IOCON_LED1 IOCON_PIO2_10
#define IOCON_LED1_FUNC IOCON_FUNC0

#define LED2 1, 3
#define IOCON_LED2 IOCON_PIO1_3
#define IOCON_LED2_FUNC IOCON_FUNC1

// Low Side Contactor
#define FSAE_CTR_LOW_SWTCH 2, 8
#define FSAE_IOCON_CTR_LOW_SWTCH IOCON_PIO2_8

// High Side Contactor
#define FSAE_CTR_HIGH_SWTCH 2, 7
#define FSAE_IOCON_CTR_HIGH_SWTCH IOCON_PIO2_7

// Fault Pin
#define FSAE_FAULT_GPIO 3, 0
#define FSAE_IOCON_FAULT_GPIO IOCON_PIO3_0

// Charge Enable Pin
#define FSAE_CHARGE_ENABLE_GPIO 3, 2
#define FSAE_IOCON_CHARGE_ENABLE_GPIO IOCON_PIO3_2

// DCDC Enable Pin
#define FSAE_DC_DC_ENABLE_GPIO 2, 0
#define FSAE_IOCON_DC_DC_ENABLE_GPIO IOCON_PIO2_0

// DCDC Fault Pin
#define FSAE_DC_DC_FAULT_GPIO 2, 6
#define FSAE_IOCON_DC_DC_FAULT_GPIO IOCON_PIO2_6

// Fan 1 Pin
#define FSAE_FAN_1_PIN 1, 4
#define FSAE_IOCON_FAN_1_PIN IOCON_PIO1_4

// Fan 1 is timer 3
#define MATCH_REGISTER_FAN_1 3
#define ENABLE_PWM_FAN_1 (1 << MATCH_REGISTER_FAN_1)

// Fan 2 Pin
#define FSAE_FAN_2_PIN 1, 3
#define FSAE_IOCON_FAN_2_PIN IOCON_PIO1_3

// Fan 2 is timer 2
#define MATCH_REGISTER_FAN_2 2
#define ENABLE_PWM_FAN_2 (1 << MATCH_REGISTER_FAN_2)

// Mystery magic numbers, talk to Jorge if curious
#define FAN_TIMER_PRESCALE 18
#define MATCH_REGISTER_FAN_PWM_CYCLE 0

// Cycle time for PWM
#define FAN_PWM_CYCLE 100

// Fan off means it runs at (Cycle + 1)ms, which never happens
#define FAN_OFF_DUTY_RATIO -1
#define FAN_TIMER_OFF (FAN_PWM_CYCLE - FAN_OFF_DUTY_RATIO)

// Fan on means it starts its run at (Cycle - ratio)ms, which happens here 90% of the time
#define FAN_ON_DUTY_RATIO 90
#define FAN_TIMER_ON (FAN_PWM_CYCLE - FAN_ON_DUTY_RATIO)

// Unused pins
#define PIN_37 3, 1
#define PIN_37_IOCON IOCON_PIO3_1
#define PIN_37_PIO_FUNC IOCON_FUNC0
#define PIN_35 1, 2
#define PIN_35_IOCON IOCON_PIO1_2
#define PIN_35_PIO_FUNC IOCON_FUNC1
#define PIN_34 1, 1
#define PIN_34_IOCON IOCON_PIO1_1
#define PIN_34_PIO_FUNC IOCON_FUNC1
#define PIN_33 1, 0
#define PIN_33_IOCON IOCON_PIO1_0
#define PIN_33_PIO_FUNC IOCON_FUNC1
#define PIN_32 0, 11
#define PIN_32_IOCON IOCON_PIO0_11
#define PIN_32_PIO_FUNC IOCON_FUNC1
#define PIN_31 2, 11
#define PIN_31_IOCON IOCON_PIO2_11
#define PIN_31_PIO_FUNC IOCON_FUNC0
#define PIN_30 1, 10
#define PIN_30_IOCON IOCON_PIO1_10
#define PIN_30_PIO_FUNC IOCON_FUNC0
#define PIN_29 0, 10
#define PIN_29_IOCON IOCON_PIO0_10
#define PIN_29_PIO_FUNC IOCON_FUNC1


#endif
