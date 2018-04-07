#include "state_vcu_root.h"

void enter_vcu_state_root(void) {
  printf("[VCU FSM : ROOT] ENTERED!\r\n");
  printf("[VCU FSM : ROOT] Currently waiting for MASTER RESET press.\r\n");
}

void update_vcu_state_root(void) {
  // Master Reset is normally high.
  if (!READ_PIN(MASTER_RST)) {
    printf("[VCU FSM : ROOT] MASTER RESET pressed.\r\n");
    set_vcu_state(VCU_STATE_LV);
  }
}
