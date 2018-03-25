
#include "error_state.h"
#include "state.h"

void setupErrorState() {
    changeErrorState(NO_ERROR_NO_ESD_STATE);
}

bool boardHeartbeatsGood() {
    if (HAL_GetTick() - board_heartbeats_state.bms >= CAN_BMS_HEARTBEAT_FAULT_DURATION) {
        return false;
    }
    // if (HAL_GetTick() - board_heartbeats_state.frontCanNode > CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION) {
    //     return false;
    // }

    return true;
}

void printHeartbeatFailures() {
    // printf("LAST BMS: %llu, NOW: %llu", board_heartbeats_state.bms, HAL_GetTick());
    // if (HAL_GetTick() - board_heartbeats_state.bms >= CAN_BMS_HEARTBEAT_FAULT_DURATION) {
    //     printf("\r\nBMS HEARTBEAT WAS NOT RECEIVED IN TIMELY FASHION\r\n");
    // }
    // if (HAL_GetTick() - board_heartbeats_state.frontCanNode > CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION) {
    //     printf("\r\nFRONT CAN NODE HEARTBEAT WAS NOT RECEIVED IN TIMELY FASHION\r\n");
    // }
}

void throwErrorIfBadHeartbeats() {
    if (!boardHeartbeatsGood()) {
        changeErrorState(HEARTBEAT_ERROR_STATE);
        printHeartbeatFailures();
    }
}

void updateErrorState() {
    updateGateFaults(); // Update the fault gates for our uses
    throwErrorIfBadHeartbeats(); // Check for heartbeats

    switch(errorState) {
        case NO_ERROR_NO_ESD_STATE:
            updateInNoErrorNoESDState();
            break;

        case NO_ERROR_STATE:
            updateInNoErrorState();
            break;

        case NO_ERROR_WITH_TSMS_STATE:
            updateInNoErrorWithTSMSState();
            break;

        case LOOP_ERROR_STATE:
            updateInLoopErrorState();
            break;

        case HEARTBEAT_ERROR_STATE:
            updateInHeartbeatErrorState();
            break;

        default:
            break;
    }
}

void changeErrorState(uint8_t newState) {
    switch (newState) {
        case NO_ERROR_NO_ESD_STATE:
            initInNoErrorNoESDState();
            break;

        case NO_ERROR_STATE:
            errorState = newState;
            initInNoErrorState();
            break;

        case NO_ERROR_WITH_TSMS_STATE:
            errorState = newState;
            initInNoErrorWithTSMSState();
            break;

        case LOOP_ERROR_STATE:
            errorState = newState;
            initInLoopErrorState();
            break;

        case HEARTBEAT_ERROR_STATE:
            errorState = newState;
            initInHeartbeatErrorState();
            break;

        default:
            break;
    }
}

void initInNoErrorState() {
    printf("\r\nAS OF NOW, NO DETECTABLE ERRORS, TSMS EXCLUDED\r\n");
}

void initInNoErrorNoESDState() {
    printf("\r\nAS OF NOW, NO DETECTABLE ERRORS, ESD & TSMS EXCLUDED\r\n");
}

void initInNoErrorWithTSMSState() {
    printf("\r\nAS OF NOW, NO DETECTABLE ERRORS, TSMS INCLUDED\r\n");
}

void initInLoopErrorState() {
    // When we get into this state, immediately send the car into the fault state
    changeCarMode(CAR_STATE_CONTACTOR_FAULT);
}

void initInHeartbeatErrorState() {
    changeCarMode(CAR_STATE_HEARTBEAT_FAULT);
}

void updateInNoErrorState() {

    if (anyGateFaultsTripped()) {
        changeErrorState(LOOP_ERROR_STATE);
        printGateFaults();
    }
}

void updateInNoErrorNoESDState() {
    if (anyGateNonESDFaultsTripped()) {
        changeErrorState(LOOP_ERROR_STATE);
        printGateFaults();
    }
}

void updateInNoErrorWithTSMSState() {

    if (anyGateFaultsTripped() || gate_faults.tsms_fault) {
        changeErrorState(LOOP_ERROR_STATE);
        printGateFaults();
        if (gate_faults.tsms_fault) {
            printf("\t[FAULT]: TSMS FAULT\r\n");
        }
    }
}

void updateInLoopErrorState() {
    if (!anyGateNonESDFaultsTripped()) {
        // Can be caused by a master reset on the gate driver such that now we might have no faults
        printf("\r\nTRANSITIONING OUT OF ERROR STATE: PROBABLY A MASTER RESET\r\n");
        changeErrorState(NO_ERROR_NO_ESD_STATE);
        changeCarMode(CAR_STATE_LV_ONLY);
    }
}

void updateInHeartbeatErrorState() {
    // No way out of this state
}
