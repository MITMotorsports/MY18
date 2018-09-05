#include "fault.h"

FATAL_FAULTS_T fatal_faults             = {};
RECOVERABLE_FAULTS_T recoverable_faults = {};

// TODO: Move the fatal and recoverable fault questions to fault.c/h
#define VS_EQ(name) (get_vcu_state() == VCU_STATE_ ## name)
#define VS_NEQ(name) !VS_EQ(name)

bool update_fatal_faults(void) {
  fatal_faults.gate      = any_fatal_gate_faults();
  fatal_faults.precharge = any_fatal_precharge_fault();
  fatal_faults.contactor = any_fatal_contactor_faults();
  fatal_faults.conflict  = any_fatal_conflict_faults();

  return VS_NEQ(ROOT) && fatal_faults.gate ||
         fatal_faults.precharge            ||
         fatal_faults.contactor            ||
         fatal_faults.conflict;
}

bool update_recoverable_faults(void) {
  recoverable_faults.gate      = any_recoverable_gate_fault();
  recoverable_faults.heartbeat = any_recoverable_heartbeat_faults();
  recoverable_faults.conflict  = any_recoverable_conflict_faults();
  recoverable_faults.contactor = any_recoverable_contactor_faults();

  return VS_NEQ(ROOT) && recoverable_faults.gate ||
         recoverable_faults.heartbeat            ||
         recoverable_faults.conflict             ||
         recoverable_faults.contactor;
}

void handle_fatal_fault(void) {
  // Print the status of all monitored faults
  // TODO/HACK: Reenable discharge sequence.
  // // Just in case we lose a CAN msg, do this multiple times
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  // Make sure the driver can avoid death.
  set_brake_valve(false);
  lock_brake_valve();
  disable_controls();

  openLowSideContactor();
  openHighSideContactor();

  printf("[FAULT : HANDLER : FATAL] Need external recovery.\r\n");
}

void handle_recoverable_fault(void) {
  if (recoverable_faults.gate || get_vcu_state() != VCU_STATE_DRIVING) {
    sendMotorOffCmdMsg();
  }
  else {
    sendTorqueCmdMsg(0);
  }

  // Make sure the driver can avoid death.
  set_brake_valve(false);
  lock_brake_valve();
  disable_controls();
}

void handle_test_fault(void) {
  const Time_T reset_timeout = 5000;
  Time_T in_time             = HAL_GetTick();

  printf("[TEST] Reached end of test. Resetting in %dms...\r\n", reset_timeout);

  while (HAL_GetTick() - in_time < reset_timeout) {
    print_gate_faults(false);
  }

  NVIC_SystemReset();
}
