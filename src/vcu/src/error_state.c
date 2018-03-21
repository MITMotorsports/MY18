
#include "error_state.h"
#include "state.h"

void setupErrorState() {
    errorState = NO_ERROR_START_STATE;
}

void updateErrorState() {
    updateGateFaults(); // Update the fault gates for our uses

    switch(errorState) {
        case NO_ERROR_START_STATE:
            updateInStartState();
            break;

        case NO_ERROR_NOMINAL_STATE:
            updateInNominalState();
            break;

        case LOOP_ERROR_STATE:
            updateInLoopErrorState();
            break;

        default:
            break;
    }
    // In here, we want to check the value of all the contactor commands 
    // and look for disagreements with actual contactor openness states

    // If in the start state,
    //      If latch errors -> ERROR!!!

    // If instead in the nominal state
    //      If any latch errors OR any disagreements in contactor states
    // --> ERROR!!!

    // Then the error will show up in the state transition in the main loop
    // changeCarMode(CAR_STATE_CONTACTOR_FAULT);
}

void changeErrorState(uint8_t newState) {
    switch (newState) {
        case NO_ERROR_START_STATE:
            errorState = newState;
            initInStartState();
            break;

        case NO_ERROR_NOMINAL_STATE:
            errorState = newState;
            initInNominalState();
            break;

        case LOOP_ERROR_STATE:
            errorState = newState;
            initInLoopErrorState();
            break;

        default:
            break;
    }
}

void initInStartState() {}

void initInNominalState() {}

void initInLoopErrorState() {
    // When we get into this state, immediately send the car into the fault state
    changeCarMode(CAR_STATE_CONTACTOR_FAULT);
}

void updateInStartState() {

    bool tripped = anyGateFaultsTripped();
    bool anyContactorCloseCmds = (commanded_contactors.low_side == CONTACTOR_CLOSED 
        || commanded_contactors.high_side == CONTACTOR_CLOSED);

    if (tripped) {
        // If we haven't sent out any close contactor commands yet and we have trips,
        // we need to error immediately
        changeErrorState(LOOP_ERROR_STATE);
        printGateFaults();
    
    } else if (!tripped && anyContactorCloseCmds) {
        // If we have not gotten trips and are commanding any contactors to close,
        // send us to the nominal state
        changeErrorState(NO_ERROR_NOMINAL_STATE);
    }
}

void updateInNominalState() {

    bool tripped = anyGateFaultsTripped();
    bool lowSideContactorDisagreement = false;//commanded_contactors.low_side != sensed_contactors.low_side;
    bool highSideContactorDisagreement = false;//commanded_contactors.high_side != sensed_contactors.high_side;

    if (tripped || lowSideContactorDisagreement || highSideContactorDisagreement) {
        changeErrorState(LOOP_ERROR_STATE);
    }

    if (tripped) {
        printGateFaults();
    }

    if (lowSideContactorDisagreement) {
        printf("\r\n[ERROR]: LOW SIDE CONTACTOR DISAGREEMENT BETWEEN COMMAND AND SENSOR\r\n");
    }

    if (highSideContactorDisagreement) {
        printf("\r\n[ERROR]: HIGH SIDE CONTACTOR DISAGREEMENT BETWEEN COMMAND AND SENSOR\r\n");
    }

}

void updateInLoopErrorState() {

}
