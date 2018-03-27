#include "fault_contactors.h"

volatile Contactors_T contactors = {};

// TODO: Implement requested vs measured mismatch contactor fault.

bool any_fatal_contactor_faults(void) {
  return any_fatal_contactor_weld_faults();
}

bool any_fatal_contactor_weld_faults(void) {
  return false;
}
