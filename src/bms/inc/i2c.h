#ifndef _I2C_H_
#define _I2C_H_

#include "pins.h"
#include "chip.h"



void I2C_Init();

//Master read = Slave address + 1, write s= Slave address + 0
//data is MSB first
float I2C_Read(uint8_t addr, int len);

void I2C_Write(uint8_t addr, uint16_t* content);

#endif