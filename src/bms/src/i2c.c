#include "i2c.h"

static I2C_XFER_T xfer;
static float rx_buf[20];
static uint16_t tx_buf[20];

void I2C_Init() {
	Chip_SYSCTL_PeriphReset(RESET_I2C0); // Reset the I2C Peripheral
	Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_SDA, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_SCL, IOCON_FUNC1);

	Chip_I2C_INIT(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);
	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = tx_buf;
	xfer.rxBuff = rx_buf;
}

// void Set_Slave_Addr(uint8_t addr) {
// 	xfer.slaveAddr = addr;
// }

float I2C_Read(uint8_t addr, int len) {
	xfer.slaveAddr = addr;

	Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, 0x0, xfer.rxBuff, len);

	return xfer.rxBuff[0];
}

void I2C_Write(uint8_t addr, uint16_t* content) {

	int i;
	for (i = 0; i < sizeof(content)/sizeof(content[0]); i++) {
		tx_buf[i] = content[i];
	}

	xfer.txSz = i;
	xfer.slaveAddr = addr;
	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);


}