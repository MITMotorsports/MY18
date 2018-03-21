#ifndef __ERROR_STATE_H
#define __ERROR_STATE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include <stdbool.h>
#include "fault_gates.h"

// ERROR STATES FOR THE LOOP OPEN STATE MACHINE
#define NO_ERROR_START_STATE 		0 			// NO CONTACTOR COMMANDS YET
#define NO_ERROR_NOMINAL_STATE		1			// CONTACTOR STATE AVAILABLE
#define LOOP_ERROR_STATE    		2			// A FAULT OCCURED

void setupErrorState();
void updateErrorState();

void changeErrorState(uint8_t newState);

void initInStartState();
void initInNominalState();
void initInLoopErrorState();

void updateInStartState();
void updateInNominalState();
void updateInLoopErrorState();

uint8_t errorState;

#endif
