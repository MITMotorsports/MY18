#include "state.h"
#include <stdbool.h>

void update_brake_throttle_conflict(VCU_BreakandThrottle break_throttle, VCU_ImplausibilityConflict* implaus_conflict);
void update_implausibility(VCU_BreakandThrottle brake_throttle, VCU_ImplausibilityConflict *implaus_conflict, uint32_t msg_ticks, uint32_t current_ticks);
