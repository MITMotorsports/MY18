#include "state_vcu_lv.h"

void initLVOnly() {
  printf("[VCU FSM : LV] ENTERED!\r\n");
  printf("[VCU FSM : LV] Currently waiting for DRIVER RESET press.\r\n");
}

void loopLVOnly() {
  if (buttons.DriverReset) {
    printf("[VCU FSM : LV] DRIVER RESET pressed.\r\n");
    set_vcu_state(VCU_STATE_PRECHARGING);
  }
}
