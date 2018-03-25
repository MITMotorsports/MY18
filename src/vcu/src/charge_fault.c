
#include "charge_fault.h"
#include "contactors.h"

void initChargeFault() {
  printf("\r\n[PRECHARGE FAULT] A FULL CAR POWER CYCLE IS REQUIRED.\r\n");

  openLowSideContactor();
  openHighSideContactor();
}

void loopChargeFault() {
  openLowSideContactor();
  openHighSideContactor();
}
