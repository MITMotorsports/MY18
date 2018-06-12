#ifndef _I2C_H_
#define _I2C_H_

#include "pins.h"
#include "chip.h"



void I2C_Init();

//Master read = Slave address + 1, write s= Slave address + 0
//data is MSB first
void I2C_Write(uint16_t* content);

uint8_t* I2C_Read(uint8_t pointer_byte, uint8_t len);

void I2C_Set_Address(uint8_t addr);

#endif