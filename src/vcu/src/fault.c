#include "fault.h"



void handle_fatal_fault(void) {
  // Print the status of all monitored faults
  // TODO/HACK: Reenable discharge sequence.
  // // Just in case we lose a CAN msg, do this multiple times
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  openLowSideContactor();
  openHighSideContactor();

  printf("[FAULT : HANDLER : FATAL] NEED POWER CYCLE.\r\n");
  while (1) {
    print_gate_faults(false);
    HAL_Delay(1000);
  }
}

void handle_test_fault(void) {
  const Time_T reset_timeout = 1000;
  Time_T in_time = HAL_GetTick();

  printf("[TEST] Reached end of test. Resetting in %dms...\r\n", reset_timeout);
  while (HAL_GetTick() - in_time < reset_timeout) {
    print_gate_faults(false);
  }

  NVIC_SystemReset();
}
