#include "brake_throttle.h"

// See IC1.13.4 in the rules
#define IMPLAUSIBILITY_REPORT_MS 100

// 10% of 1000, see IC1.13.5 in the rules
#define IMPLAUSIBILITY_THROTTLE_TRAVEL 100

// 25% of 1000, see EV2.5 in the rules
#define CONFLICT_BEGIN_THROTTLE_TRAVEL 250

// 5% of 1000, see EV2.5.1 in the rules
#define CONFLICT_END_THROTTLE_TRAVEL 50

#define CONFLICT_BRAKE_RAW 600

void update_brake_throttle_conflict(VCU_BrakeAndThrottle brake_throttle, VCU_ImplausibilityConflict* implaus_conflict) {
  if (implaus_conflict->actual_implausibility) {
    // Checking conflict is pointless if implausibility
    return;
  }

  // Take minimum
  const uint16_t accel = (brake_throttle.accel_1 > brake_throttle.accel_2) ? brake_throttle.accel_2 : brake_throttle.accel_1;

  bool curr_conflict = implaus_conflict->has_brake_throttle_conflict;

  if (curr_conflict) {
    // Conflict state: Remove conflict if throttle < 5% travel (EV2.5.1)
    implaus_conflict->has_brake_throttle_conflict =  accel >= CONFLICT_END_THROTTLE_TRAVEL;
  }

  bool brake_engaged = brake_throttle.brake_1 > CONFLICT_BRAKE_RAW;

  bool throttle_engaged = accel > CONFLICT_BEGIN_THROTTLE_TRAVEL;

  implaus_conflict->has_brake_throttle_conflict = brake_engaged && throttle_engaged;
}

void update_implausibility(VCU_BrakeAndThrottle brake_throttle, VCU_ImplausibilityConflict *implaus_conflict, uint32_t msg_ticks, uint32_t current_ticks) {
  uint16_t max_travel = (brake_throttle.accel_1 > brake_throttle.accel_2) ? brake_throttle.accel_1 : brake_throttle.accel_2;
  uint16_t min_travel = (brake_throttle.accel_1 > brake_throttle.accel_2) ? brake_throttle.accel_2 : brake_throttle.accel_1;
  bool curr_implausible =  max_travel - min_travel >= IMPLAUSIBILITY_THROTTLE_TRAVEL;
  bool prev_implausible = implaus_conflict->observed_implausibility;

  if (!curr_implausible) {
    // No implausibility, so clear everything
    implaus_conflict->observed_implausibility = false;
    implaus_conflict->actual_implausibility = false;
    return;
  }

  implaus_conflict->observed_implausibility = true;

  if (!prev_implausible) {
    // This is the first implausibility
    implaus_conflict->implausibility_ticks = msg_ticks;
    return;
  }

  // See if we have had this implausibility for a while
  uint32_t time_elapsed = msg_ticks - implaus_conflict->implausibility_ticks;
  bool should_report = time_elapsed > implaus_conflict->implausibility_ticks;
  implaus_conflict->actual_implausibility = should_report;
}
