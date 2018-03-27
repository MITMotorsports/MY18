#include "state_vcu_rtd.h"

void initReadyToDrive() {
  printf("[VCU FSM] READY TO DRIVE\r\n");
}

void loopReadyToDrive() {
  if (buttons.RTD &&
      ((pedalbox.brake_1 > CONFLICT_BRAKE_RAW) ||
       (pedalbox.brake_2 > CONFLICT_BRAKE_RAW))) {
    set_vcu_state(VCU_STATE_DRIVING);
  }
}
