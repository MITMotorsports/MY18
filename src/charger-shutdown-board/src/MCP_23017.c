#include "MCP_23017.h"

void init_MCP2307(void){
	send_i2c(0, MCP23017_IOCON);
	send_i2c_2(0,0, MCP23017_GPIO); 
	send_i2c_2(0x1F,0,MCP23017_IODIR); //lower 5 bits button input
	send_i2c(0x1F, MCP23017_GPPU); //pullup lower 5 bits
	send_i2c_2(0,0,MCP23017_GPINTEN);
	read_i2c(MCP23017_PORTA );
	read_i2c(MCP23017_PORTB );
}

//To read GPIO call read_i2c(MCP23017_GPIO) for port a and read_i2c(MCP23017_GPIO+1) for port B
//

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


