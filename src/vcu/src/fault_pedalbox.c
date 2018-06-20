#include "fault_pedalbox.h"

Conflicts_T conflicts        = {};
volatile Pedalbox_T pedalbox = {};


bool any_recoverable_conflict_faults(void) {
  update_conflicts();
  update_implausibility();
  return conflicts.brake_accel || conflicts.accel;
}

bool any_fatal_conflict_faults(void) {
  return false;
}

void update_conflicts() {
  if (conflicts.brake_accel) {
    // Clear conflict if accel was released.
    // printf("[FAULT : PEDALBOX : BRAKE_ACCEL] Conflict.\r\n");
    if ((pedalbox_min(accel) < PEDALBOX_ACCEL_RELEASE) &&
        (pedalbox.brake_2 < PEDALBOX_BRAKE_BEGIN)) {
      printf("[FAULT : PEDALBOX : BRAKE_ACCEL] Potential release.\r\n");
      conflicts.brake_accel = false;
    }
    else {
      // If conflict not cleared, keep it as is.
      return;
    }
  }

  bool brake_engaged = pedalbox.brake_2 > PEDALBOX_BRAKE_BEGIN;
  bool accel_engaged = pedalbox_min(accel) > PEDALBOX_ACCEL_BEGIN;

  conflicts.brake_accel = brake_engaged && accel_engaged;
}

void update_implausibility() {
  conflicts.any_errs = conflicts.fcn.accel_1_over  ||
                      conflicts.fcn.accel_1_under ||
                      conflicts.fcn.accel_2_over  ||
                      conflicts.fcn.accel_2_under ||
                      conflicts.fcn.brake_1_over  ||
                      conflicts.fcn.brake_1_under ||
                      conflicts.fcn.brake_2_over  ||
                      conflicts.fcn.brake_2_under;

  bool implausibility = ((pedalbox_max(accel) - pedalbox_min(accel)) >
                        PEDALBOX_ACCEL_IMPLAUSIBLE) ||
                        conflicts.any_errs;

  // There is an implausibility if the range of accel meaasurements is above the
  // implausibility threshold.

  static Time_T last_bad = 0;

  if (implausibility) {
    if (conflicts.observed_implausibility) {
      if (HAL_GetTick() - conflicts.last_implausibility >
          IMPLAUSIBILITY_TIMEOUT) {
        conflicts.accel = true;
      }
    }
    else {
      conflicts.last_implausibility = HAL_GetTick();
      last_bad = HAL_GetTick();
    }
  }
  else {
    if (conflicts.observed_implausibility) {
      last_bad = HAL_GetTick();
    }
    else {
      if (HAL_GetTick() - last_bad > 1000) {
        conflicts.accel = false;
      }
    }
  }

  conflicts.observed_implausibility = implausibility;
}

int32_t get_pascals(int16_t brake_reading) {
  // Same conversion as the dash
  int32_t psi = (2019 * brake_reading) / 1000 - 188;

  // Convert to pascals
  int32_t pascals = 689476 * psi / 100;

  return pascals;
}
