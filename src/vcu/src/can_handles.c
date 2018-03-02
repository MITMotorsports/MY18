
#include "can_handles.h"

void handleBrakeThrottleMsg(Frame* msg) {
	can0_FrontCanNodeBrakeThrottle_T unpacked_msg;
	unpack_can0_FrontCanNodeBrakeThrottle(msg, &unpacked_msg);

	brake_and_throttle_state.accel_1 = unpacked_msg.accel_1;
	brake_and_throttle_state.accel_2 = unpacked_msg.accel_2;
	brake_and_throttle_state.brake_1 = unpacked_msg.brake_1;
	brake_and_throttle_state.brake_2 = unpacked_msg.brake_2;

	board_heartbeats_state.frontCanNode = HAL_GetTick();
}

void sendTorqueCmdMsg(int16_t torque) {
	can0_MC_Command_T msg;
	msg.torque = torque;
	msg.speed = 0;
	msg.direction_is_counterclockwise = 0;
	msg.inverter_enabled = 1;
	msg.discharge_enabled = 0;
	msg.torque_limit = 0;

	can0_MC_Command_Write(&msg);
}
