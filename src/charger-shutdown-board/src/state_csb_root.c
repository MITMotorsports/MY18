
#include "state_csb_root.h"

void enter_csb_state_root(void) {
  Board_Print("[CSB FSM : ROOT] ENTERED!\n");

  lcd_clear();
  lcd_set_cursor(0, 0);
  lcd_print("ROOT");
}

void update_csb_state_root(void) {
  if (!any_gate_fault()) {
    set_csb_state(CSB_STATE_PRECHARGE);
  }
}
