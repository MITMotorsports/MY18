#include "state_csb_precharge.h"

static uint32_t prechargeTimeout;
static uint32_t DEAD_RECKON_TIME=10000;

void enter_csb_precharge(void){
	Board_Print("Entered Precharge!\n");
	prechargeTimeout=msTicks;
}

void update_csb_precharge(void){
	if(msTicks-prechargeTimeout>DEAD_RECKON_TIME){
		Board_Pin_Set(PIN_PRECHARGE, 1);
		set_csb_state(CSB_STATE_CHARGE);
	}	
}
