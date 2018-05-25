#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn = 24000000;


can0_ChargerStatus1_T status;


static I2C_XFER_T xfer;
static uint8_t i2c_rx_buf[20];
static uint8_t i2c_tx_buf[20];

inline void delay (uint32_t time){
	uint32_t wait=msTicks;
	while(msTicks-wait <time){}
}


void SysTick_Handler(void) {
  msTicks++;
}

void I2C_IRQHandler(void)
{
	if (Chip_I2C_IsMasterActive(I2C0)) {
		Chip_I2C_MasterStateHandler(I2C0);
	}
	else {
		Chip_I2C_SlaveStateHandler(I2C0);
	}
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

	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = i2c_tx_buf;
	xfer.rxBuff = i2c_rx_buf;
	xfer.slaveAddr = MCP23017_ADDRESS;
	bool b=1;
	Board_Print("I2C Initialized\n");
	uint32_t wait=msTicks;
	//no more init
	init_MCP2307();
//	MCP2307_writeGPIOAB(0xFFFF);
	send_i2c(0, MCP23017_GPIO +1);	
	init_lcd();
	
	command(0x32);
	command(0x32);//turn off white bars
	write_str("voltage:",8);
	command(LCD_SETDDRAMADDR | 0x40 ); //move the cursor to 2nd row
	write_str("current:",8);
//Update voltage and current values
	command(LCD_SETDDRAMADDR |  9);
	write_str("270", 3);
	command(LCD_SETDDRAMADDR | 9 + 0x40);
	write_str("008", 3);
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
