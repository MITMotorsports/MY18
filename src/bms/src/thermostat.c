#include "thermostat.h"

#define CONFIG_REGISTER (RES_9_BITS | FAULT_TOL_2 | OS_ACTIVE_HIGH | MODE_COMPARATOR | THERMO_ACTIVE)
#define SHUTDOWN (CONFIG_REGISTER | THERMO_SHUTDOWN)

static bool is_positive(uint8_t num) {
	return !(num & (1<<8));
}

int8_t Thermo_Read(void) {
	//int len = (fl) ? 2 : 1;
	uint8_t raw_data;
	raw_data = I2C_Read(POINTER_TEMP, 1)[0];

	uint8_t u_temp = (raw_data) & (MASK_SIGN_BIT); //no decimal reading here, use mask for 

	int8_t s_temp = is_positive(raw_data) ? u_temp : -u_temp;
	
	return s_temp;
}

void Thermo_Init(void) {
	uint8_t data[2];
	I2C_Set_Address(THERMO_ADDR);
	data[0] = POINTER_CONFIG;
	data[1] = CONFIG_REGISTER;
	I2C_Write(data);
}