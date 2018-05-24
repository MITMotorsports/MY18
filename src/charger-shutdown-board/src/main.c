#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn = 24000000;


can0_ChargerStatus1_T status;

#define SCL 0, 4
#define SDA 0, 5
#define MCP23017_ADDRESS 0x20
#define MCP23017_IODIR 0x00
#define MCP23017_GPIO 0x12
#define MCP23017_IOCON 0x0A
#define MCP23017_GPINTEN 0x04
#define MCP23017_PORTA 0x00
#define MCP23017_PORTB 0x01

static I2C_XFER_T xfer;
static uint8_t i2c_rx_buf[20];
static uint8_t i2c_tx_buf[20];

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
	send_i2c(0xFF, MCP23017_GPIO );	
       	while(1){
//		advance_csb_state();
//		can_receive();

			if(msTicks-wait>500){
				send_i2c(0xFF, MCP23017_GPIO);
				/*

				b=!b;
				if(b){
					Board_Print("Low\r\n");
					MCP2307_writeGPIOAB(0);
				}
				else{
					MCP2307_writeGPIOAB(0xFFFF);
					Board_Print("High\r\n");
				}
			*/	
	
				wait=msTicks;
			}
	}


 	
	return 0;
}


void init_MCP2307(void){
	send_i2c(0, MCP23017_IOCON);
	send_i2c_2(0,0, MCP23017_GPIO);
	send_i2c_2(0,0,MCP23017_IODIR);
//	send_i2c(0, MCP23017_IODIRB)
	send_i2c_2(0,0,MCP23017_GPINTEN);
	read_i2c(MCP23017_PORTA );
	read_i2c(MCP23017_PORTB );
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


void read_i2c(uint8_t slave_register){
	Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, slave_register, i2c_rx_buf, 1);
}

void send_i2c_2(uint8_t slave_data1, uint8_t slave_data2, uint8_t slave_register){
	i2c_tx_buf[0] = slave_register;
	i2c_tx_buf[1] = slave_data1;
	i2c_tx_buf[2] = slave_data2;

	xfer.txSz = 3;

	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}
void send_i2c(uint8_t slave_data, uint8_t slave_register){
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
