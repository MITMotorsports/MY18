
#include "ready_to_drive.h"

void initReadyToDrive() {
	printf("\r\nREADY TO DRIVE\r\n");
}

void loopReadyToDrive() {
	if (button_presses.RTD && 
		(brake_and_throttle_state.brake_1 > CONFLICT_BRAKE_RAW || 
		brake_and_throttle_state.brake_2 > CONFLICT_BRAKE_RAW)) {

		changeCarMode(CAR_STATE_DRIVING);
	}
}
