#include "fault_contactors.h"

volatile Contactors_T contactors = {};

// TODO: Implement requested vs measured mismatch contactor fault. Possibly on
// BMS too.

bool any_fatal_contactor_faults(void) {
  return any_fatal_contactor_mismatch_faults();
}

bool any_recoverable_contactor_faults(void) {
  return false;
}

const Time_T L_side_timeout = 200;

bool any_fatal_contactor_mismatch_faults(void) {
  static Time_T last_L_mismatch_time = 0;
  static bool   last_L_mismatch      = false;

  bool L_mismatch = (READ_PIN(L_CONTACTOR) != READ_PIN(L_CONTACTOR_STATUS));

  if (L_mismatch) {
    if (last_L_mismatch) {
      if (HAL_GetTick() - last_L_mismatch_time > L_side_timeout) {
        printf("[FAULT : CONTACTORS] LOW side mismatch timeout!\r\n");
        return true;
      }
    }
    else {
      last_L_mismatch_time = HAL_GetTick();
    }
  }

  last_L_mismatch = L_mismatch;

  return false;
}

void print_contactor_state(bool force) {
  printf("L SIDE MONITOR %d\r\n", contactors.L_contactor_closed);
}
