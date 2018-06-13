#include "thermostat.h"

#define CONFIG_REGISTER (RES_9_BITS | FAULT_TOL_2 | OS_ACTIVE_HIGH | MODE_COMPARATOR | THERMO_ACTIVE)
#define SHUTDOWN (CONFIG_REGISTER | THERMO_SHUTDOWN)

bool is_positive(uint8_t num) {
	return !(num & (1<<8));
}

int16_t Thermo_Read(void) {
	//int len = (fl) ? 2 : 1;
	uint8_t* raw_data = I2C_Read(POINTER_TEMP, 2);
	uint16_t joined = 0;

	joined = raw_data[0] << 8 | raw_data[1];

	uint16_t u_temp = (joined * 10) >> 8;
	int16_t s_temp = is_positive(raw_data[0]) ? u_temp : -u_temp;
	
	Board_Println_BLOCKING("Ambient:");
	Board_PrintNum(s_temp,10);
	Board_Println_BLOCKING("first byte: ");
	Board_PrintNum(raw_data[0],10);
	Board_PrintNum(raw_data[1],10);
	return s_temp;
}

void Thermo_Init(void) {
	uint8_t data[2];
	I2C_Set_Address(THERMO_ADDR);
	data[0] = POINTER_CONFIG;
	data[1] = CONFIG_REGISTER;
	I2C_Write(data);
	Board_Println("Finished Thermostat init...");
}