#include "fault.h"



void handle_fatal_fault(void) {
  // Print the status of all monitored faults

  // Just in case we lose a CAN msg, do this multiple times
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  openLowSideContactor();
  openHighSideContactor();

  printf("[FAULT : HANDLER : FATAL] NEED POWER CYCLE.\r\n");
  while (1) {
    print_gate_faults();
    HAL_Delay(1000);
  }
}
