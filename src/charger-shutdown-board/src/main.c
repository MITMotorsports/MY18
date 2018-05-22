#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn = 24000000;


can0_ChargerStatus1_T status;

#define SCL 0, 4
#define SDA 0, 5
#define MCP23017_ADDRESS 0x20
#define MCP23017_IODIRA 0x00
#define MCP23017_IODIRB 0x01
#define MCP23017_GPIOA 0x12
#define MCP23017_GPIOB 0x13


static I2C_XFER_T xfer;
static uint8_t i2c_rx_buf[20];
static uint8_t i2c_tx_buf[20];

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

	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = i2c_tx_buf;
	xfer.rxBuff = i2c_rx_buf;
	xfer.slaveAddr = MCP23017_ADDRESS;


	Board_Print("I2C Initialized\n");

	//no more init
	init_MCP2307();
	MCP2307_writeGPIOAB(0xFFFF);
	while(1){
//		advance_csb_state();
//		can_receive();
 	}
	return 0;
}


void init_MCP2307(void){
	send_i2c_lcd_byte(0xFF,MCP23017_IODIRA); //all inputs on port A
       	send_i2c_lcd_byte(0xFF,MCP23017_IODIRB); //all inputs on port B
}

/*
uint16_t MCP2307_readGPIOAB() {
  uint16_t ba = 0;
  uint8_t a;

  // read the current GPIO output latches
  WIRE.beginTransmission(MCP23017_ADDRESS | i2caddr);
  wiresend(MCP23017_GPIOA);	
  WIRE.endTransmission();
  
  WIRE.requestFrom(MCP23017_ADDRESS | i2caddr, 2);
  a = wirerecv();
  ba = wirerecv();
  ba <<= 8;
  ba |= a;

  return ba;
}
*/

void MCP2307_writeGPIOAB(uint16_t ba) {
	send_i2c_lcd_byte(ba&0xFF,MCP23017_GPIOA); 
	send_i2c_lcd_byte(ba >> 8,MCP23017_GPIOB); 
}
void poll_lcd(uint8_t slave_register){
	Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, slave_register, i2c_rx_buf, 1);
}

void send_i2c_lcd_byte(uint8_t slave_data, uint8_t slave_register){
	i2c_tx_buf[0] = slave_register;
	i2c_tx_buf[1] = slave_data;
	xfer.txSz = 2;

	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
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
