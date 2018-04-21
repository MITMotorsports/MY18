#include "fault.h"

// TODO: Move the fatal and recoverable fault questions to fault.c/h
#define VS_EQ(name) (get_vcu_state() == VCU_STATE_ ## name)
#define VS_NEQ(name) !VS_EQ(name)

bool any_fatal_faults(void) {
  return VS_NEQ(ROOT) && any_fatal_gate_faults() ||
         any_fatal_precharge_fault()             ||
         any_fatal_contactor_faults()            ||
         any_fatal_conflict_faults();
}

bool any_recoverable_faults(void) {
  return VS_NEQ(ROOT) && any_recoverable_gate_fault() ||
         any_recoverable_heartbeat_faults()           ||
         any_recoverable_conflict_faults()            ||
         any_recoverable_contactor_faults();
}

void handle_fatal_fault(void) {
  // Print the status of all monitored faults
  // TODO/HACK: Reenable discharge sequence.
  // // Just in case we lose a CAN msg, do this multiple times
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  HAL_Delay(200);

  openLowSideContactor();
  openHighSideContactor();

  printf("[FAULT : HANDLER : FATAL] NEED POWER CYCLE.\r\n");
  while (1) {
    print_gate_faults(false);
    sendHeartbeatMsg();
  }
}

void handle_recoverable_fault(void) {
  sendTorqueCmdMsg(0);
  resetDrivingValues();
}

void handle_test_fault(void) {
  const Time_T reset_timeout = 5000;
  Time_T in_time = HAL_GetTick();

  printf("[TEST] Reached end of test. Resetting in %dms...\r\n", reset_timeout);
  while (HAL_GetTick() - in_time < reset_timeout) {
    print_gate_faults(false);
  }

  NVIC_SystemReset();
}
