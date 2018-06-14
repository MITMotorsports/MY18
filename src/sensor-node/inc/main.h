#include "adc_spi.h"
#include "uart.h"
#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "canmgr.h"
#include "speed.h"

extern volatile uint32_t msTicks;

void Serial_Init(uint32_t baudrate);
