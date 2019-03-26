#include "fault_gates.h"

GateFaults_T gates;
static bool const BELIEVE_IMD = true;

// Gate faults occur when a pin goes LOW.
void update_gate_status(void) {
  bool old_imd = gates.imd_gate;

  gates.sdn      = !READ_PIN(SDN);
  gates.sdn_gate = !READ_PIN(SDN_GATE);
  gates.bms_gate = !READ_PIN(BMS_GATE);
  // gates.imd_gate = !READ_PIN(IMD_GATE);
  gates.imd_gate = !READ_PIN(FAKE_IMD);
  gates.bpd_gate = !READ_PIN(BPD_GATE);

  if (old_imd != gates.imd_gate) {
    printf("[IMD] Is now %d!\r\n", gates.imd_gate);
  }
}

bool any_all_gate_fault(void) {
  update_gate_status();

  return gates.sdn      ||
         gates.sdn_gate ||
         gates.bms_gate ||
         (BELIEVE_IMD && gates.imd_gate) ||
         gates.bpd_gate;
}

bool any_recoverable_gate_fault(void) {
  update_gate_status();

  return gates.sdn_gate;
}

bool any_recoverable_transient_gate_fault(void) {
  update_gate_status();

  return gates.sdn;
}

bool any_fatal_gate_faults(void) {
  update_gate_status();

  static Time_T last_time = 0;
  static bool   last_val  = 0;

  bool retval = gates.bms_gate ||
                (BELIEVE_IMD && gates.imd_gate) ||
                gates.bpd_gate;
  // I like my indents to work well.

  if (retval) {
    if (last_val) {
      if (HAL_GetTick() - last_time > 5) {
        return true;
      }
    }
    else {
      last_time = HAL_GetTick();
    }
  }

  last_val = retval;

  return false;
}

void print_gate_faults(bool force) {
  update_gate_status();

  static GateFaults_T last_gate_status = {};

  if (!force) {
    bool change = gates.sdn      != last_gate_status.sdn      ||
                  gates.sdn_gate != last_gate_status.sdn_gate ||
                  gates.bms_gate != last_gate_status.bms_gate ||
                  gates.imd_gate != last_gate_status.imd_gate ||
                  gates.bpd_gate != last_gate_status.bpd_gate;

    if (!change) return;
  }

  last_gate_status = gates;

  if (any_all_gate_fault()) {
    printf("[GATE FAULT] THE FOLLOWING FAULT GATES WERE TRIPPED:\r\n");

    if (gates.sdn) {
      printf("\t- SDN (NON GATE) FAULT\r\n");
    }

    if (gates.sdn_gate) {
      printf("\t- SDN GATE FAULT\r\n");
    }

    if (gates.bms_gate) {
      printf("\t- BMS GATE FAULT\r\n");
    }

    if (gates.imd_gate) {
      printf("\t- IMD GATE FAULT\r\n");
    }

    if (gates.bpd_gate) {
      printf("\t- BPD GATE FAULT\r\n");
    }
  }
  else {
    printf("[GATE FAULT] NO GATE FAULTS WERE TRIPPED.\r\n");
  }
}
