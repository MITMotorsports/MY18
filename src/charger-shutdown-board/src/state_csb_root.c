
#include "state_csb_root.h"

void enter_csb_state_root(void){
	Board_Print("[CSB FSM : ROOT] ENTERED!\n");
}


void update_csb_state_root(void){
	if(!any_gate_fault() && bms_state.L_contactor_closed) {
		set_csb_state(CSB_STATE_PRECHARGE);
	}
}
