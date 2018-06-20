#include "soc.h"
#include "eeprom_config.h"

void SOC_Init(BMS_PACK_STATUS_T *ps) {
	ps->state_of_charge = EEPROM_LoadCCPage_Num(SOC_EEPROM_INDEX);
	// ps->state_of_charge = 100;
	init_soc = ps->state_of_charge;
}

void SOC_Estimate(BMS_PACK_STATUS_T *ps) {
	ps->state_of_charge = init_soc - (((ps->pack_energy_wH*100)/(MAX_CHARGE)));
	if(ps->state_of_charge > 100) {
		ps->state_of_charge = 100;
	}
	if(msTicks - count > SOC_WRITE_PERIOD) {
		SOC_Write(ps);
		Board_PrintNum_BLOCKING(ps->state_of_charge,10);
	}

}

void SOC_Write(BMS_PACK_STATUS_T* ps) {
	EEPROM_WriteCCPage_Num(SOC_EEPROM_INDEX, ps->state_of_charge);
	uint8_t echo = EEPROM_LoadCCPage_Num(SOC_EEPROM_INDEX);
	Board_PrintNum_BLOCKING(echo,10);
	count = msTicks;
}

void SOC_Max(BMS_PACK_STATUS_T* ps) {
	ps->state_of_charge = 100;
	SOC_Write(ps);
}
