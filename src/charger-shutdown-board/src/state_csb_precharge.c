
#include "state_csb_precharge.h"

static uint32_t prechargeTimeout;
static uint32_t DEAD_RECKON_TIME = 10000;

void enter_csb_state_precharge(void){
	Board_Print("[CSB FSM : PRECHARGE] ENTERED!\n");
	prechargeTimeout = msTicks;
}

void update_csb_state_precharge(void){
	if(msTicks-prechargeTimeout>DEAD_RECKON_TIME){
    // After a succificent amount of time has been spent in precharge,
    // proceed to close high side contactor and prepare to turn on charger
		Board_Pin_Set(PIN_PRECHARGE, 1);
		set_csb_state(CSB_STATE_CHARGE);
	}
}
