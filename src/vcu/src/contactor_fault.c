#include "contactor_fault.h"
#include "driving.h"
#include "contactors.h"

void initContactorFault() {
  // Just in case we loose a CAN msg, do this multiple times
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();
  sendMotorOffCmdMsg();

  resetDrivingValues(); // from the driving file

  openLowSideContactor();
  openHighSideContactor();

  printf("\r\n[ERROR]: CONTACTORS OPENED AS A RESULT OF A HARDWARE FAULT\r\n");
  printf("\r\nTO ACCOUNT FOR THIS, PERFORM A MASTER RESET AND THEN A DRIVER RESET\r\n");

  changeCarMode(CAR_STATE_LV_ONLY); // NOW WE'LL NEED A MASTER RESET
}

void loopContactorFault() {}
