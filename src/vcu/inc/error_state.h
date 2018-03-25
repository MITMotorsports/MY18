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
#define HEARTBEAT_ERROR_STATE           4

#define CAN_BMS_HEARTBEAT_FAULT_DURATION                                400 // millis
#define CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION             400         // millis

void throwErrorIfBadHeartbeats();
bool boardHeartbeatsGood();
void printHeartbeatFailures();

void setupErrorState();
void updateErrorState();

void changeErrorState(uint8_t newState);


void initInNoErrorNoESDState();
void initInNoErrorState();
void initInNoErrorWithTSMSState();
void initInLoopErrorState();
void initInHeartbeatErrorState();

void updateInNoErrorNoESDState();
void updateInNoErrorState();
void updateInNoErrorWithTSMSState();
void updateInLoopErrorState();
void updateInHeartbeatErrorState();

uint8_t errorState;

#endif // ifndef __ERROR_STATE_H
