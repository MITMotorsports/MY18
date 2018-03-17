
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

void handleMCVoltageMsg(Frame* msg) {
	can0_MCVoltage_T unpacked_msg;
	unpack_can0_MCVoltage(msg, &unpacked_msg);

	mc_voltage.busVoltage = unpacked_msg.busVoltage;
	mc_voltage.outVoltage = unpacked_msg.outVoltage;
	mc_voltage.VAB_Vd_Voltage = unpacked_msg.VAB_Vd_Voltage;
	mc_voltage.VBC_Vq_Voltage = unpacked_msg.VBC_Vq_Voltage;

	board_heartbeats_state.mc = HAL_GetTick();
}

void handleBMSHeartbeatMsg(Frame* msg) {
	can0_BMSHeartbeat_T unpacked_msg;
	unpack_can0_BMSHeartbeat(msg, &unpacked_msg);

	// Load into local struct if needed ...

	board_heartbeats_state.bms = HAL_GetTick();
}


void sendTorqueCmdMsg(int16_t torque, int16_t break_and_throttle_conflict) {
	can0_MCCommand_T msg;
	msg.torque = torque;
	msg.speed = 0;
	msg.direction_is_counterclockwise = 0;
	msg.inverter_enabled = 1;
	msg.discharge_enabled = 0;
	msg.torque_limit = 0;

	can0_MCCommand_Write(&msg);
}
