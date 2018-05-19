#include "thermostat.h"

#define CONFIG_REGISTER (RES_9_BITS | FAULT_TOL_2 | OS_ACTIVE_HIGH | MODE_COMPARATOR | THERMO_ACTIVE)
#define SHUTDOWN (CONFIG_REGISTER | THERMO_SHUTDOWN)

void Thermostat_Init(void) {
	I2C_Write(THERMO_ADDR | WRITE, CONFIG_REGISTER);

	//TODO: hyst and os limits for thermostat
}

float Thermostat_Read(void) { //first few bits are for decimal values
	return I2C_Read(THERMO_ADDR | READ, 1);
}

void Thermostat_Shutdown(void) {
	I2C_Write(THERMO_ADDR | WRITE, SHUTDOWN);
}