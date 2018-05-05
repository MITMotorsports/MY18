#include "state_csb.h"

static CSB_STATE_T currentState;

void init_csb_state(void){
	set_csb_state(CSB_STATE_ROOT);
}

CSB_STATE_T set_csb_state(CSB_STATE_T newState){
	switch(newState){
	case CSB_STATE_ROOT:
		enter_csb_state_root();
		break;

	case  CSB_STATE_PRECHARGE:
		enter_csb_state_precharge();
		break;

	case CSB_STATE_CHARGE:
		enter_csb_state_charge();
		break;
	
	default:
		return;
	}
}

void advance_csb_state(void){
	switch(get_csb_state()){
	case CSB_STATE_ROOT:
		update_csb_state_root();
		break;

	case CSB_STATE_PRECHARGE:
		update_csb_state_precharge();
		break;
	
	case CSB_STATE_CHARGE:
		update_csb_state_charge();
		break;
	
	default:
		return;
	}
}

CSB_STATE_T get_csb_state(void){
	return currentState;
}
