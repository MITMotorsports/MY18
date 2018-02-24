#ifndef __STATE_H
#define __STATE_H

#include "stm32f2xx_hal.h"

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

} VCU_BreakandThrottle;

// ACTUAL OBJECTS

VCU_BoardHeartbeats board_heartbeats_state;

VCU_BreakandThrottle break_and_throttle_state;

void initVCUState();

#endif