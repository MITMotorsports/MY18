#include "state_error.h"

static ERROR_STATE_T currentState = NO_ERROR_STATE;

// Implicitly initialize error structs to false (no errors).
RECOVERABLE_ERROR_T recoverable_errors = {};
FATAL_ERROR_T fatal_errors             = {};


void init_error_state(void) {
  set_error_state(NO_ERROR_STATE);
  recoverable_errors = {};
  fatal_errors       = {};
}

ERROR_STATE_T set_error_state(ERROR_STATE_T newState) {
  if (currentState == FATAL_ERROR_STATE) {
    printf("[ERROR FSM] Attempted to exit from FATAL_ERROR_STATE! Ignoring...");
    return currentState;
  }

  switch (newState) {
  case NO_ERROR_STATE:
    enter_no_error_state();
    break;

  case RECOVERABLE_ERROR_STATE:
    enter_recoverable_error_state();
    break;

  case FATAL_ERROR_STATE:
    enter_fatal_error_state();
    break;
  }

  currentState = newState;
  return currentState;
}

void transition_error_state(void) {
  switch (currentState) {
  case NO_ERROR_STATE:
    update_no_error_state();
    break;

  case RECOVERABLE_ERROR_STATE:
    update_recoverable_error_state();
    break;

  case FATAL_ERROR_STATE:
    update_fatal_error_state();
    break;
  }
}

void enter_no_error_state(void) {
  printf("[ERROR FSM] Entered NO_ERROR_STATE!\r\n");
}

void enter_recoverable_state(void) {
  printf("[ERROR FSM] Entered RECOVERABLE_ERROR_STATE!\r\n");
}

void enter_fatal_state(void) {
  printf("[ERROR FSM] Entered FATAL_ERROR_STATE!\r\n");
}

ERROR_STATE_T current_error_state(void) {
  return currentState;
}

bool boardHeartbeatsGood(void) {
  // TODO: remove of course.
  return true;

  if (HAL_GetTick() - heartbeats.bms >= CAN_BMS_HEARTBEAT_FAULT_DURATION) {
    return false;
  }

  if (HAL_GetTick() - heartbeats.frontCanNode >
      CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION) {
    return false;
  }

  return true;
}

//
// void printHeartbeatFailures(void) {
//   // printf("LAST BMS: %llu, NOW: %llu", heartbeats.bms,
//   // HAL_GetTick());
//   // if (HAL_GetTick() - heartbeats.bms >=
//   // CAN_BMS_HEARTBEAT_FAULT_DURATION) {
//   //     printf("\r\nBMS HEARTBEAT WAS NOT RECEIVED IN TIMELY FASHION\r\n");
//   // }
//   // if (HAL_GetTick() - heartbeats.frontCanNode >
//   // CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION) {
//   //     printf("\r\nFRONT CAN NODE HEARTBEAT WAS NOT RECEIVED IN TIMELY
//   // FASHION\r\n");
//   // }
// }
//
// void throwErrorIfBadHeartbeats(void) {
//   if (!boardHeartbeatsGood()) {
//     set_error_state(HEARTBEAT_ERROR_STATE);
//     printHeartbeatFailures();
//   }
// }
//
// void update_error_state(void) {
//   updateGateFaults();          // Update the fault gates for our uses
//   throwErrorIfBadHeartbeats(); // Check for heartbeats
//
//   switch (currentState) {
//   case NO_ERROR_NO_ESD_STATE:
//     updateInNoErrorNoESDState();
//     break;
//
//   case NO_ERROR_STATE:
//     updateInNoErrorState();
//     break;
//
//   case NO_ERROR_WITH_TSMS_STATE:
//     updateInNoErrorWithTSMSState();
//     break;
//
//   case LOOP_ERROR_STATE:
//     updateInLoopErrorState();
//     break;
//
//   case HEARTBEAT_ERROR_STATE:
//     updateInHeartbeatErrorState();
//     break;
//
//   default:
//     break;
//   }
// }
//
// void set_error_state(ERROR_STATE_T newState) {
//   switch (newState) {
//   case NO_ERROR_NO_ESD_STATE:
//              currentState = newState;
//     initInNoErrorNoESDState();
//     break;
//
//   case NO_ERROR_STATE:
//     currentState = newState;
//     initInNoErrorState();
//     break;
//
//   case NO_ERROR_WITH_TSMS_STATE:
//     currentState = newState;
//     initInNoErrorWithTSMSState();
//     break;
//
//   case LOOP_ERROR_STATE:
//     currentState = newState;
//     initInLoopErrorState();
//     break;
//
//   case HEARTBEAT_ERROR_STATE:
//     currentState = newState;
//     initInHeartbeatErrorState();
//     break;
//
//   default:
//     break;
//   }
// }
//
// const inline ERROR_STATE_T current_error_state(void) {
//      return currentState;
// }
//
// void initInNoErrorState(void) {
//   printf("\r\nAS OF NOW, NO DETECTABLE ERRORS, TSMS EXCLUDED\r\n");
// }
//
// void initInNoErrorNoESDState(void) {
//   printf("\r\nAS OF NOW, NO DETECTABLE ERRORS, ESD & TSMS EXCLUDED\r\n");
// }
//
// void initInNoErrorWithTSMSState(void) {
//   printf("\r\nAS OF NOW, NO DETECTABLE ERRORS, TSMS INCLUDED\r\n");
// }
//
// void initInLoopErrorState(void) {
//   // When we get into this state, immediately send the car into the fault
// state
//   set_vcu_state(VCU_STATE_CONTACTOR_FAULT);
// }
//
// void initInHeartbeatErrorState(void) {
//   set_vcu_state(VCU_STATE_HEARTBEAT_FAULT);
// }
//
// void updateInNoErrorState(void) {
//   if (anyGateFaultsTripped()) {
//     set_error_state(LOOP_ERROR_STATE);
//     printGateFaults();
//   }
// }
//
// void updateInNoErrorNoESDState(void) {
//   if (anyGateNonESDFaultsTripped()) {
//     set_error_state(LOOP_ERROR_STATE);
//     printGateFaults();
//   }
// }
//
// void updateInNoErrorWithTSMSState(void) {
//   if (anyGateFaultsTripped() || gate_faults.tsms_fault) {
//     set_error_state(LOOP_ERROR_STATE);
//     printGateFaults();
//
//     if (gate_faults.tsms_fault) {
//       printf("\t[FAULT]: TSMS FAULT\r\n");
//     }
//   }
// }
//
// void updateInLoopErrorState(void) {
//   if (!anyGateNonESDFaultsTripped()) {
//     // Can be caused by a master reset on the gate driver such that now we
// might
//     // have no faults
//     printf("\r\nTRANSITIONING OUT OF ERROR STATE: PROBABLY A MASTER
// RESET\r\n");
//     set_error_state(NO_ERROR_NO_ESD_STATE);
//     set_vcu_state(VCU_STATE_LV_ONLY);
//   }
// }
//
// void updateInHeartbeatErrorState(void) {
//   // No way out of this state
// }
