#include "rules.h"

#include <stdbool.h>

// From rules, see IC1.13.4 in the rules
#define IMPLAUSIBILITY_REPORT_MS 100

// 10% of 1000, see IC1.13.5 in the rules
#define IMPLAUSIBILITY_THROTTLE_TRAVEL 100

// 25% of 1000, see EV2.5 in the rules
#define CONFLICT_BEGIN_THROTTLE_TRAVEL 250

// 5% of 1000, see EV2.5.1 in the rules
#define CONFLICT_END_THROTTLE_TRAVEL 50

#define CONFLICT_BRAKE_RAW 600

bool check_implausibility(uint16_t accel_1, uint16_t accel_2);

void Rules_update_implausibility(ADC_Input_T *adc, Rules_State_T *rules, uint32_t msTicks) {
  // TODO make these actually work with some reorganization of input.c to absorb transform
  uint16_t accel_1 = 0;
  uint16_t accel_2 = 0;

  bool curr_implausible = check_implausibility(accel_1, accel_2);
  bool prev_implausible = rules->implausibility_observed;

  if (!curr_implausible) {
    // No implausibility, so clear everything
    rules->implausibility_observed = false;
    rules->implausibility_reported = false;
    return;
  }

  rules->implausibility_observed = true;

  if (!prev_implausible) {
    // This is the first implausibility
    rules->implausibility_time_ms = msTicks;
    return;
  }

  // See if we have had this implausibility for a while
  uint32_t time_elapsed = msTicks - rules->implausibility_time_ms;
  bool should_report = time_elapsed > IMPLAUSIBILITY_REPORT_MS;
  rules->implausibility_reported = should_report;
}

void Rules_update_conflict(Input_T *input, Rules_State_T *rules) {
  ADC_Input_T *adc = input->adc;
  if (rules->implausibility_reported) {
    // Checking conflict is pointless if implausibility
    return;
  }

  // TODO make these actually work with some reorganization of input.c
  const uint16_t accel_1 = 0;
  const uint16_t accel_2 = 0;

  // Take minimum
  const uint16_t accel = (accel_1 > accel_2) ? accel_2 : accel_1;

  bool curr_conflict = rules->has_brake_throttle_conflict;

  if (curr_conflict) {
    // Conflict state: Remove conflict if throttle < 5% travel (EV2.5.1)
    bool next_conflict = accel >= CONFLICT_END_THROTTLE_TRAVEL;
    rules->has_brake_throttle_conflict = next_conflict;
    return;
  }

  // TODO Figure out why in MY17 this was set to be the same
  const uint16_t brake = adc->brake_1;

  // TODO adjust this setting based on LV voltage
  bool brake_engaged = brake > CONFLICT_BRAKE_RAW;

  bool throttle_engaged = accel > CONFLICT_BEGIN_THROTTLE_TRAVEL;

  rules->has_brake_throttle_conflict = brake_engaged && throttle_engaged;
}

bool check_implausibility(uint16_t accel_1_travel, uint16_t accel_2_travel) {
  uint16_t max_travel = (accel_1_travel > accel_2_travel) ? accel_1_travel : accel_2_travel;
  uint16_t min_travel = (accel_1_travel > accel_2_travel) ? accel_2_travel : accel_1_travel;
  return max_travel - min_travel >= IMPLAUSIBILITY_THROTTLE_TRAVEL;
}
