
#ifndef __FAULT_GATES_H
#define __FAULT_GATES_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "state.h"
#include <stdbool.h>

void initFaultGates();

void updateGateFaults(); // gate faults in state.h

bool anyGateFaultsTripped();
bool anyGateNonESDFaultsTripped();

void printGateFaults();

#endif
