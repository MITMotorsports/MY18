#include "state_csb_precharge.h"

static uint32_t prechargeTimeout;
static uint32_t DEAD_RECKON_TIME=5000;

void enter_csb_state_precharge(void){
	Board_Print("Entered Precharge!\n");
	prechargeTimeout=msTicks;
}

void update_csb_state_precharge(void){
	if(msTicks-prechargeTimeout>DEAD_RECKON_TIME){
		Board_Pin_Set(PIN_PRECHARGE, 1); //After a succificent amount of time has been spent in precharge, proceed to close high side contactor and prepare to turn on charger
		set_csb_state(CSB_STATE_CHARGE);
	}	

	bool any_fault = !Board_Pin_Read(PIN_BMS_FAULT) ||
			 !Board_Pin_Read(PIN_IMD_IN)    ||
			 !Board_Pin_Read(PIN_INTERLOCK);
	if(any_fault){
		set_csb_state(CSB_STATE_ROOT);
		Board_Print("Fault tripped, entering root state\n");
	}
	
}
