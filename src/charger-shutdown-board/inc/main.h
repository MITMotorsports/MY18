#ifndef __MAIN_H
#define __MAIN_H
#include <string.h>
#include "chip.h"
#include "board.h"
#include "config.h"
#include "pins.h"
#include "uart.h"
#include "CANlib.h"



extern volatile uint32_t msTicks;

#define CAN_INIT() init_can0_csb()
extern uint8_t can_data[8];

#endif //ifndef __main_h
