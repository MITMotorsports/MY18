#include "fault.h"



void handle_fatal_fault(void) {
  // Just in case we loose a CAN msg, do this multiple times
  print_gate_faults();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  openLowSideContactor();
  openHighSideContactor();

  printf("\r\n[FATAL FAULT HANDLER] A FULL CAR POWER CYCLE IS REQUIRED. FORCED HANG.\r\n");
  while (1) {}
}
