
#include "state.h"

void initVCUState(void) {
	// BOARD HEARTBEATS
	board_heartbeats_state.frontCanNode = HAL_GetTick();

	// BREAK AND THROTTLE
	break_and_throttle_state.accel_1 = 0;
	break_and_throttle_state.accel_2 = 0;
	break_and_throttle_state.brake_1 = 0;
	break_and_throttle_state.brake_2 = 0;

	implaus_conflict_state.has_brake_throttle_conflict = false;
	implaus_conflict_state.observed_implausibility = false;
	implaus_conflict_state.actual_implausibility = false;
	implaus_conflict_state.implausibility_ticks;
}
