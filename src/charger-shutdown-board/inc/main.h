#ifndef __MAIN_H
#define __MAIN_H
#include <string.h>
#include "pins.h"
#include "uart.h"
#include "CANlib.h"



extern volatile uint32_t msTicks;

void can_receive(void);
void can_receive_status_1(void);

void init_MCP2307(void); //initializes port expander
void MCP2307_writeGPIOAB(uint16_t ba); 



static Frame can_input;
extern can0_ChargerStatus1_T status;

#define CAN_INIT() init_can0_csb()

#endif //ifndef __main_h
