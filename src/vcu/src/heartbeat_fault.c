
#include "heartbeat_fault.h"
#include "driving.h"
#include "contactors.h"

void initHeartbeatFault() {
  // Just in case we loose a CAN msg, do this multiple times
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  openLowSideContactor();
  openHighSideContactor();

  printf("\r\n[HEARTBEAT FAULT] A FULL CAR POWER CYCLE IS REQUIRED.\r\n");
}

void loopHeartbeatFault() {
  openLowSideContactor();
  openHighSideContactor();
}
