
#include "state.h"

void initVCUState() {
	// BOARD HEARTBEATS
	board_heartbeats_state.frontCanNode = HAL_GetTick();

	// BREAK AND THROTTLE
	break_and_throttle_state.accel_1 = 0;
	break_and_throttle_state.accel_2 = 0;
	break_and_throttle_state.brake_1 = 0;
	break_and_throttle_state.brake_2 = 0;
}


