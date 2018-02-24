
#include "can_handles.h"

void handleBreakThrottleMsg(Frame* msg) {
	can0_FrontCanNodeBrakeThrottle_T unpacked_msg;
	unpack_can0_FrontCanNodeBrakeThrottle(msg, &unpacked_msg);

	break_and_throttle_state.accel_1 = unpacked_msg.accel_1;
	break_and_throttle_state.accel_2 = unpacked_msg.accel_2;
	break_and_throttle_state.brake_1 = unpacked_msg.brake_1;
	break_and_throttle_state.brake_2 = unpacked_msg.brake_2;

	board_heartbeats_state.frontCanNode = HAL_GetTick();
}

