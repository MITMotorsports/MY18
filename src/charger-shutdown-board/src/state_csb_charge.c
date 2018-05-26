#include "state_csb_charge.h"

can0_ChargerCommand_T msg;

uint32_t can_wait;

Frame frame;

uint16_t voltage;
uint16_t current;


void enter_csb_state_charge(void){
	Board_Print("Entered Charge!\n");
	
	can_wait=0;

	msg.enable=1;
	msg.powerReference=1000;
	msg.VoltageLimit=3000; //Max voltage set to 300 volts 	
	msg.CurrentLimit=100;

}

void update_csb_state_charge(void){
	bool any_fault = !Board_Pin_Read(PIN_BMS_FAULT) ||
			 !Board_Pin_Read(PIN_IMD_IN)    ||
			 !Board_Pin_Read(PIN_INTERLOCK);
	if(any_fault){
		set_csb_state(CSB_STATE_ROOT);
		Board_Print("Fault tripped, entering root state\n");
	}
	if(msTicks-can_wait>750){
		can0_ChargerCommand_Write(&msg);
		can_wait=msTicks;

		voltage=status.DCVoltage;
		voltage/=10;
		current=status.DCCurrent;
		current/=10;
		Board_Print("\r\nVoltage: ");
		Board_PrintNum(voltage, 10);
		Board_Print("\r\n Current: ");
		Board_PrintNum(current, 10);

	       	lcd_command(LCD_SETDDRAMADDR |  9); //move cursor
		lcd_print_num(voltage);
		
		lcd_command(LCD_SETDDRAMADDR | 9 + 0x40); //cursor again
		lcd_print_num(current);
	}
}

