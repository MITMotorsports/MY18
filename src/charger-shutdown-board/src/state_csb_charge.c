#include "state_csb_charge.h"

can0_ChargerCommand_T msg;

uint32_t can_wait;

Frame frame;

uint16_t voltage;
uint16_t current;

void enter_csb_state_charge(void){
	Board_Print("[CSB FSM : CHARGE] ENTERED!\n");
}


void update_csb_state_charge(void){
	send_ChargerCommand(true);

	lcd_set_cursor(9, 0);
	lcd_print_num(voltage, 10);

	lcd_set_cursor(9, 1);
	lcd_print_num(current, 10);
}
