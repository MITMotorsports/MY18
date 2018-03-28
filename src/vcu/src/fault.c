#include "fault.h"



void handle_fatal_fault(void) {
  // Print the status of all monitored faults

  // Just in case we lose a CAN msg, do this multiple times
  print_gate_faults();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  openLowSideContactor();
  openHighSideContactor();

  while (1) {
    Error_Handler("[FAULT : HANDLER : FATAL] NEED POWER CYCLE.\r\n");
  }
}
