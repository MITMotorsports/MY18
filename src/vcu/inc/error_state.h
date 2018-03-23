#ifndef __ERROR_STATE_H
#define __ERROR_STATE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include <stdbool.h>
#include "fault_gates.h"

// ERROR STATES FOR THE LOOP OPEN STATE MACHINE
#define NO_ERROR_NO_ESD_STATE       0
#define NO_ERROR_STATE              1 
#define NO_ERROR_WITH_TSMS_STATE    2
#define LOOP_ERROR_STATE            3

void setupErrorState();
void updateErrorState();

void changeErrorState(uint8_t newState);

void initInNoErrorNoESDState();
void initInNoErrorState();
void initInNoErrorWithTSMSState();
void initInLoopErrorState();

void updateInNoErrorNoESDState();
void updateInNoErrorState();
void updateInNoErrorWithTSMSState();
void updateInLoopErrorState();

uint8_t errorState;

#endif
