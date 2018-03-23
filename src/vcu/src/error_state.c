
#include "error_state.h"
#include "state.h"

void setupErrorState() {
    changeErrorState(NO_ERROR_NO_ESD_STATE);
}

void updateErrorState() {
    updateGateFaults(); // Update the fault gates for our uses

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
