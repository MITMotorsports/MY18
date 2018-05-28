
#include "state_csb_root.h"

void enter_csb_state_root(void){
	Board_Print("Entered Root!\n");
}

void update_csb_state_root(void){
	bool no_fault = Board_Pin_Read(PIN_BMS_FAULT)&&Board_Pin_Read(PIN_IMD_IN)&&Board_Pin_Read(PIN_INTERLOCK);
	if(no_fault) //if the top 3 fets are open low side contactors are open and precharge has been entered
		set_csb_state(CSB_STATE_PRECHARGE);
}
