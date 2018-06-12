#include "thermostat.h"

#define CONFIG_REGISTER (RES_12_BITS | FAULT_TOL_2 | OS_ACTIVE_HIGH | MODE_COMPARATOR | THERMO_ACTIVE)
#define SHUTDOWN (CONFIG_REGISTER | THERMO_SHUTDOWN)

static bool is_positive(uint8_t num) {
	return !(num & (1<<8));
}

int8_t Thermo_Read(void) {
	//int len = (fl) ? 2 : 1;
	uint8_t* raw_data = I2C_Read(POINTER_TEMP, 2);

	uint8_t raw_data_dec = raw_data[1] >> 4; //these bits are reversed
	//uint16_t dec = 0;
	uint8_t power = 0x10;
	uint8_t i;

	// while(raw_data_dec > 0) {
	// 	if(raw_data_dec & 0x01) {
	// 		dec += (1.0)/((float)power) * 10000;
	// 	}
	// 	power = power >> 1;
	// 	raw_data_dec >> 1;
	// }

	uint8_t u_temp = (raw_data[0]) & (MASK_SIGN_BIT); //no decimal reading here, use mask for 

	int8_t s_temp = is_positive(raw_data[0]) ? u_temp : -u_temp;
	
	Board_Println_BLOCKING("Thermo:");
	Board_PrintNum(s_temp,10);
	Board_Print_BLOCKING('.');
	Board_PrintNum(raw_data_dec,10);

	return s_temp;
}

void Thermo_Init(void) {
	uint8_t data[2];
	Board_Println("Before set address");
	I2C_Set_Address(THERMO_ADDR);
	data[0] = POINTER_CONFIG;
	data[1] = CONFIG_REGISTER;
	Board_Println("Before i2c write");
	I2C_Write(data);

	Board_Println("Finished Thermostat init...");
}