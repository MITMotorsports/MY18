
#include "state.h"
#include "lv_only.h"
#include "precharge.h"
#include "charge_fault.h"
#include "ready_to_drive.h"
#include "driving.h"

void initVCUState(void) {
	// BOARD HEARTBEATS
	board_heartbeats_state.frontCanNode = HAL_GetTick();
	board_heartbeats_state.dash = HAL_GetTick();
	board_heartbeats_state.bms = HAL_GetTick();
    board_heartbeats_state.mc = HAL_GetTick();

	// BRAKE AND THROTTLE
	brake_and_throttle_state.accel_1 = 0;
	brake_and_throttle_state.accel_2 = 0;
	brake_and_throttle_state.brake_1 = 0;
	brake_and_throttle_state.brake_2 = 0;

	// IMPLAUSIBILITY AND BRAKE CHECK
	implaus_conflict_state.has_brake_throttle_conflict = false;
	implaus_conflict_state.observed_implausibility = false;
	implaus_conflict_state.actual_implausibility = false;
	implaus_conflict_state.implausibility_ticks = 0;
	
	// DC BUS VOLTAGE OF MOTOR CONTROLLER
    mc_voltage.busVoltage = 0;
    mc_voltage.outVoltage = 0;
    mc_voltage.VAB_Vd_Voltage = 0;
    mc_voltage.VBC_Vq_Voltage = 0;

	// CAR Mode
	changeCarMode(CAR_STATE_LV_ONLY);
}

void changeCarMode(uint8_t newState) {
	switch (carMode) {
        case CAR_STATE_LV_ONLY:
        	carMode = newState;
            initLVOnly();
            break;
        case CAR_STATE_PRECHARGING:
        	carMode = newState;
            initPrecharge();
            break;
        case CAR_STATE_CHARGE_FAULT:
        	carMode = newState;
            initChargeFault();
            break;
        case CAR_STATE_READY_TO_DRIVE:
        	carMode = newState;
            initReadyToDrive();
            break;
        case CAR_STATE_DRIVING:
        	carMode = newState;
            initDriving();
            break;

        default:
        	printf("\r\n[WARNING]: INVALID CAR MODE CHANGE.\r\n");
            break;
    }
}
