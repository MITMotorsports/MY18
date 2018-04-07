#include "fault_pedalbox.h"

Conflicts_T conflicts = {};
volatile Pedalbox_T  pedalbox  = {};


bool any_recoverable_conflict_faults(void) {
  update_conflicts();
  update_implausibility();
  return false;
}

bool any_fatal_conflict_faults(void) {
  update_conflicts();
  update_implausibility();
  return conflicts.accel   ||
         conflicts.brake_accel;
}

void update_conflicts() {
  if (conflicts.brake_accel) {
    // Clear conflict if accel was released.
    if (pedalbox_min(accel) < PEDALBOX_ACCEL_RELEASE) {
      conflicts.brake_accel = false;
    }
    else {
      // If conflict not cleared, keep it as is.
      return;
    }
  }

  bool brake_engaged = pedalbox.brake_1 > PEDALBOX_BRAKE_BEGIN;
  bool accel_engaged = pedalbox_min(accel) > PEDALBOX_ACCEL_BEGIN;

  conflicts.brake_accel = brake_engaged && accel_engaged;
}

void update_implausibility() {
  // If there's an implausibility retain it until cleared manually.
  if (conflicts.accel) return;

  bool implausibility = (pedalbox_max(accel) - pedalbox_min(accel)) >
                        PEDALBOX_ACCEL_IMPLAUSIBLE;

  // There is an implausibility if the range of accel meaasurements is above the
  // implausibility threshold.

  if (implausibility) {
    if (conflicts.observed_implausibility) {
      if (HAL_GetTick() - conflicts.last_implausibility >
          IMPLAUSIBILITY_TIMEOUT) {
        conflicts.accel = true;
      }
    }
    else {
      conflicts.last_implausibility = HAL_GetTick();
    }
  }

  conflicts.observed_implausibility = implausibility;
}
