#include "i2c.h"

#define ACCEL_SLAVE_ADDR 0x19
#define CTRL_REG1        0x20
#define NORM_POWER_MODE  0x20
#define BUFFER_SIZE      20

//Axes
#define OUT_X_L          0x28
#define OUT_X_H          0x29
#define OUT_Y_L          0x2A
#define OUT_Y_H          0x2B
#define OUT_Z_L          0x2C
#define OUT_Z_H          0x2D
#define WHOAMI           0xf


static I2C_XFER_T xfer;
static uint8_t i2c_rx_buf[BUFFER_SIZE];
static uint8_t i2c_tx_buf[BUFFER_SIZE];
static uint8_t data[BUFFER_SIZE-1];

void I2C_IRQHandler(void) {
  if (Chip_I2C_IsMasterActive(I2C0)) {
    Chip_I2C_MasterStateHandler(I2C0);
  }
  else {
    Chip_I2C_SlaveStateHandler(I2C0);
  }
}

void I2C_Init(void) {
	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1); // SCL
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1); // SDA

	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = i2c_tx_buf;
	xfer.rxBuff = i2c_rx_buf;
	xfer.slaveAddr = ACCEL_SLAVE_ADDR;
}

static void Buffer_Clear(void) {
	return;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		i2c_rx_buf[i] = 0x0;
		i2c_tx_buf[i] = 0x0;
	}
}

static void Data_Clear(void) {
	int i;
	for(i = 0; i < BUFFER_SIZE - 1; i++) {
		data[i] = 0x0;
	}
}

uint8_t I2C_Read(uint8_t reg) {
	Buffer_Clear();
	Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, reg, i2c_rx_buf, 1);
	return i2c_rx_buf[0];
}


static void Axes_Enable(bool enable) {
	uint8_t axes_settings;
	axes_settings = I2C_Read(CTRL_REG1);

	if(enable) {
		axes_settings |= 0x07;
	} else {
		axes_settings &= 0x07;
	}

	data[0] = axes_settings;
	I2C_Write(CTRL_REG1, 1);
}

static void Set_PowerMode(uint8_t pm) {
	uint8_t curr_powermode = I2C_Read(CTRL_REG1) & 0x1f;
	data[0] = curr_powermode | pm;
	I2C_Write(CTRL_REG1, 1);
}

void Accel_Init(void) {
	Buffer_Clear();
	Data_Clear();


	Set_PowerMode(NORM_POWER_MODE);

	Axes_Enable(true);

	data[0] = 0x0; //write the following registers to 0 (default settings)
	int i;
	for(i = 0x21; i < 0x25; i++) {
		I2C_Write(i, 1);
	}

	for(i = 0x30; i < 0x37; i++) {
		I2C_Write(i, 1);
	}
}


void I2C_Write(uint8_t reg, uint8_t len) {
	// Buffer_Clear();
	i2c_tx_buf[0] = reg;

	int i;
	for (i = 0; i < len; i++) {
		i2c_tx_buf[i+1] = data[i];
	}
	Data_Clear();
	xfer.txSz = len + 1;
	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}

static float convertToG(int maxScale, int reading) {
  float result = ((float)maxScale * (float)(reading-2047))/2047;
  return result;
}


void Read_Axes(Orient_T* orient) {
	orient->x = convertToG(1000, ((I2C_Read(OUT_X_L) | I2C_Read(OUT_X_H) << 8) >> 4));
	orient->y = convertToG(1000, ((I2C_Read(OUT_Y_L) | I2C_Read(OUT_Y_H) << 8) >> 4));
	orient->z = convertToG(1000, ((I2C_Read(OUT_Z_L) | I2C_Read(OUT_Z_H) << 8) >> 4));

	// orient->x = (I2C_Read(OUT_X_L) | I2C_Read(OUT_X_H) << 8) >> 4;
	// orient->y = (I2C_Read(OUT_Y_L) | I2C_Read(OUT_Y_H) << 8) >> 4;
	// orient->z = (I2C_Read(OUT_Z_L) | I2C_Read(OUT_Z_H) << 8) >> 4;
	print("X: ");
	printNum(orient->x, 10);
	print(" Y: ");
	printNum(orient->y,10);
	print("  Z: ");
	printNum(orient->z,10);
	println("");
}


