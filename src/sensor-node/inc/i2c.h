#ifndef _I2C_H__
#define _I2C_H__

#include "chip.h"

void I2C_Init(void);

typedef struct Orient {
	int16_t x;
	int16_t y;
	int16_t z;
} Orient_T;

#endif