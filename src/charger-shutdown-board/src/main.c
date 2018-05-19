#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn = 12000000;

#define SCL 0, 4
#define SDA 0, 5
#define SLAVEADDR 0x64

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

	// LPC_SYSCTL->SYSAHBCLKCTRL |= 0x20; 	// Enable clock and power to I2C block
	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = i2c_tx_buf;
	xfer.rxBuff = i2c_rx_buf;
	xfer.slaveAddr = 0x64;


	Board_Print("I2C Initialized\n");
	//no more init
	while(1){
//		advance_csb_state();
		Board_PrintNum(msTicks,10);
 	}
	return 0;
}


