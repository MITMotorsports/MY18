
#include "ready_to_drive.h"

void initReadyToDrive() {
	printf("\r\nREADY TO DRIVE\r\n");
}

void loopReadyToDrive() {
	if (buttons.rtdBtnPressed && brake_and_throttle_state.brake_1 && brake_and_throttle_state.brake_2) {
		changeCarMode(CAR_STATE_DRIVING);
	}
}
