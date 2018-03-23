#ifndef __STATE_H
#define __STATE_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include <stdbool.h>

// DEFINITION OF CAR MODES
#define CAR_STATE_LV_ONLY 				0
#define CAR_STATE_PRECHARGING 			1
#define CAR_STATE_CHARGE_FAULT 			2
#define CAR_STATE_READY_TO_DRIVE 		3
#define CAR_STATE_DRIVING 				4
#define CAR_STATE_CONTACTOR_FAULT       5

uint8_t carMode;

typedef struct 
{
	bool bms_fault;
	bool bpd_fault;
	bool imd_fault;
	bool sdn_fault;
	bool esd_fault;
	bool NOT_IMPLEMENTED_YET_tsms_fault;

} GateFaults;

typedef struct {
	int16_t packVoltage;
} BMS_CellVoltage;

typedef struct
{
	int16_t busVoltage;
	int16_t outVoltage;
	int16_t VAB_Vd_Voltage;
	int16_t VBC_Vq_Voltage;

} MC_Voltage;

typedef struct
{
	uint32_t frontCanNode;
	uint32_t bms;
	uint32_t mc;

} VCU_BoardHeartbeats;

typedef struct
{
	int16_t accel_1;
	int16_t accel_2;
	int16_t brake_1;
	int16_t brake_2;

} VCU_BrakeAndThrottle;

typedef struct
{
	bool has_brake_throttle_conflict;
	bool observed_implausibility;
	bool actual_implausibility;
	uint32_t implausibility_ticks;
} VCU_ImplausibilityConflict;

typedef struct
{
	bool RTD;
	bool DriverReset;
	bool ScrollSelect;
} ButtonBank_Presses;

// ACTUAL OBJECTS
VCU_BoardHeartbeats board_heartbeats_state;
VCU_BrakeAndThrottle brake_and_throttle_state;
VCU_ImplausibilityConflict implaus_conflict_state;

GateFaults gate_faults;

MC_Voltage mc_voltage;

BMS_CellVoltage bms_voltage;

ButtonBank_Presses button_presses;

void initVCUState(void);

void changeCarMode(uint8_t newState);

#endif
