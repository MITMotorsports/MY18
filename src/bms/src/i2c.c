#include "i2c.h"

static I2C_XFER_T xfer;
static uint8_t rx_buf[20];
static uint8_t tx_buf[20];

void I2C_IRQHandler(void)
{
	if (Chip_I2C_IsMasterActive(I2C0)) {
		Chip_I2C_MasterStateHandler(I2C0);
	}
	else {
		Chip_I2C_SlaveStateHandler(I2C0);
	}
}

void I2C_Init(void) {
	Chip_SYSCTL_PeriphReset(RESET_I2C0); // Reset the I2C Peripheral
	Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_SDA, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_SCL, IOCON_FUNC1);
	
	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);
	
	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = tx_buf;
	xfer.rxBuff = rx_buf;

	Board_Println("Finished I2C init...");
}

void I2C_Set_Address(uint8_t addr) {
	xfer.slaveAddr = addr;
}

void I2C_Write(uint8_t* content) {

	int i;
	for (i = 0; i < sizeof(content)/sizeof(content[0]); i++) {
		tx_buf[i] = content[i];
	}

	xfer.txSz = i;
	//Board_Println("Before master send");
	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
	//Board_Println("After master send");
}

uint8_t* I2C_Read(uint8_t pointer_byte, uint8_t len) {
	Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, pointer_byte, xfer.rxBuff, len);
	return xfer.rxBuff;
}