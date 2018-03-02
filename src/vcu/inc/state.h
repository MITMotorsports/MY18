#ifndef __STATE_H
#define __STATE_H

#include "stm32f2xx_hal.h"
#include <stdbool.h>

// DEFINITION OF STRUCTURES
typedef struct
{
	uint32_t frontCanNode;

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
// ACTUAL OBJECTS

VCU_BoardHeartbeats board_heartbeats_state;

VCU_BrakeAndThrottle brake_and_throttle_state;

VCU_ImplausibilityConflict implaus_conflict_state;

void initVCUState(void);

#endif
