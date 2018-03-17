#include "state.h"
#include <stdbool.h>
#include "stdio.h"

void update_brake_throttle_conflict(VCU_BrakeAndThrottle brake_throttle, VCU_ImplausibilityConflict* implaus_conflict);
void update_implausibility(VCU_BrakeAndThrottle brake_throttle, VCU_ImplausibilityConflict *implaus_conflict, uint32_t msg_ticks);
