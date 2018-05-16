#include "state_csb_charge.h"
void enter_csb_state_charge(void){
	Board_Print("Entered Charge!\n");
}

void update_csb_state_charge(void){
	if(!Board_Pin_Read(PIN_BMS_FAULT)||!Board_Pin_Read(PIN_IMD_IN)||!Board_Pin_Read(PIN_INTERLOCK)){
		set_csb_state(CSB_STATE_ROOT);
		Board_Print("Fault tripped, entering root state");
	}

	CAN_Transmit(0x600,can_data,8); //format is sender id, 
	//Charger CAN address is 0x2FF
}
