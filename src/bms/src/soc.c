#include "soc.h"
#include "eeprom_config.h"

void SOC_Init(BMS_PACK_STATUS_T *ps) {
	ps->state_of_charge = EEPROM_LoadCCPage_Num(SOC_EEPROM_INDEX);
	init_soc = ps->state_of_charge;
	count = msTicks;
}

void SOC_Estimate(BMS_PACK_STATUS_T *ps) {
	ps->state_of_charge = init_soc - ((float)ps->pack_energy_wH/(MAX_CHARGE));

	if(ps->state_of_charge > 100) {
		ps->state_of_charge = 100;
	}
	if(msTicks - count > SOC_WRITE_PERIOD) {
		SOC_Write(ps);
	}

}

void SOC_Write(BMS_PACK_STATUS_T* ps) {
	EEPROM_WriteCCPage_Num(SOC_EEPROM_INDEX, ps->state_of_charge);
	uint32_t echo = EEPROM_LoadCCPage_Num(SOC_EEPROM_INDEX);
	Board_PrintNum(echo,10);
	count = msTicks;
}

void SOC_Max(BMS_PACK_STATUS_T* ps) {
	ps->state_of_charge = 100;
	SOC_Write(ps);
}