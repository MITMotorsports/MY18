#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn = 24000000;


can0_ChargerStatus1_T status;



void SysTick_Handler(void) {
  msTicks++;
}



int main(void) {
  	SystemCoreClockUpdate();
  	Board_GPIO_Init();
	Board_UART_Init(57600);
  	Board_Print("H e l l o\n");
  	CAN_INIT();
  	Board_Print("CAN Initialized\n");

	if(SysTick_Config(SystemCoreClock/1000))
		while(1);
	//DON'T FORGET THIS A N G E R 	
		
//	// LPC_SYSCTL->SYSAHBCLKCTRL |= 0x20; 	// Enable clock and power to I2C block

	//no more init
	init_MCP2307();
	init_lcd();
	
       	while(1){
		advance_csb_state();
		can_receive();

	}


 	
	return 0;
}


void can_receive(void){
	Can_RawRead(&can_input);
	can0_T msgForm = identify_can0(&can_input);
	switch(msgForm){
	case can0_ChargerStatus1:
		can_receive_status_1();
		break;
	default:
		break;
	}
}

void can_receive_status_1(void){
	unpack_can0_ChargerStatus1(&can_input, &status);
}
