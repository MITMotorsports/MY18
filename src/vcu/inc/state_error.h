#ifndef __STATE_ERROR_H
#define __STATE_ERROR_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include <stdbool.h>

#include "fault_gates.h"
#include "extern.h"



// ERROR STATES FOR THE LOOP OPEN STATE MACHINE
typedef enum {
  NO_ERROR_NO_ESD_STATE,
  NO_ERROR_STATE,
  NO_ERROR_WITH_TSMS_STATE,
  LOOP_ERROR_STATE,
  HEARTBEAT_ERROR_STATE,
} ERROR_STATE_T;



// CONSTANTS
#define CAN_BMS_HEARTBEAT_FAULT_DURATION 400
#define CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION 400


// INTERACTION FUNCTIONS
void init_error_state(void);
void update_error_state(void);

void set_error_state(ERROR_STATE_T newState);

const ERROR_STATE_T current_error_state(void);

void throwErrorIfBadHeartbeats(void);
bool boardHeartbeatsGood(void);
void printHeartbeatFailures(void);

// PRIVATE FUNCTIONS
void initInNoErrorNoESDState(void);
void initInNoErrorState(void);
void initInNoErrorWithTSMSState(void);
void initInLoopErrorState(void);
void initInHeartbeatErrorState(void);

void updateInNoErrorNoESDState(void);
void updateInNoErrorState(void);
void updateInNoErrorWithTSMSState(void);
void updateInLoopErrorState(void);
void updateInHeartbeatErrorState(void);



#endif // ifndef __STATE_ERROR_H
