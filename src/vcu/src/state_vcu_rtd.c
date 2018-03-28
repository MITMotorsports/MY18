#include "state_vcu_rtd.h"

void enter_vcu_state_rtd() {
  printf("[VCU FSM] READY TO DRIVE\r\n");
}

void update_vcu_state_rtd() {
  if (buttons.RTD &&
      ((pedalbox.brake_1 > CONFLICT_BRAKE_RAW) ||
       (pedalbox.brake_2 > CONFLICT_BRAKE_RAW))) {
    set_vcu_state(VCU_STATE_DRIVING);
  }
}
