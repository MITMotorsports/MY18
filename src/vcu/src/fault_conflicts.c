#include "fault_conflicts.h"

Conflicts_T conflicts = {};

bool any_recoverable_conflict_faults(void) {
  return false;
}

bool any_fatal_conflict_faults(void) {
  return false;
}

void update_conflicts() {
  if (conflicts.actual_implausibility) {
    // Checking conflict is pointless if implausibility
    return;
  }

  // Take minimum
  const uint16_t accel = MIN_DE(pedalbox.accel_1, pedalbox.accel_2);

  bool curr_conflict = conflicts.has_pedalbox_conflict;

  if (curr_conflict) {
    // Conflict state: Remove conflict if throttle < 5% travel (EV2.5.1)
    conflicts.has_pedalbox_conflict = accel >= CONFLICT_END_THROTTLE_TRAVEL;
  }

  bool brake_engaged = pedalbox.brake_1 > CONFLICT_BRAKE_RAW;

  bool throttle_engaged = accel > CONFLICT_BEGIN_THROTTLE_TRAVEL;

  conflicts.has_pedalbox_conflict = brake_engaged && throttle_engaged;
}

void update_implausibility() {
  static Time_T msg_ticks = 0;

  uint16_t max_travel = MAX_DE(pedalbox.accel_1, pedalbox.accel_2);
  uint16_t min_travel = MIN_DE(pedalbox.accel_1, pedalbox.accel_2);

  bool curr_implausible = max_travel - min_travel >=
                          IMPLAUSIBILITY_THROTTLE_TRAVEL;

  bool prev_implausible = conflicts.observed_implausibility;

  if (!curr_implausible) {
    // No implausibility, so clear everything
    conflicts.observed_implausibility = false;
    conflicts.actual_implausibility   = false;
    return;
  }

  conflicts.observed_implausibility = true;

  if (!prev_implausible) {
    // This is the first implausibility
    conflicts.implausibility_ticks = msg_ticks;
    return;
  }

  // See if we have had this implausibility for a while
  Time_T time_elapsed  = msg_ticks - conflicts.implausibility_ticks;
  bool   should_report = time_elapsed > conflicts.implausibility_ticks;
  conflicts.actual_implausibility = should_report;
}
