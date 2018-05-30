
#include "state_csb_precharge.h"

static uint32_t prechargeTimeout;
static uint32_t DEAD_RECKON_TIME = 10000;

void enter_csb_state_precharge(void){
	Board_Print("[CSB FSM : PRECHARGE] ENTERED!\n");
	prechargeTimeout = msTicks;
}


void update_csb_state_precharge(void){
	// 6 modules, 12 cells each; we divide by two because we're averaging the min
	// and max cell voltages; we muliply by 9 and divide by 10 to take 90%
	uint32_t cutoff_voltage = (bms_state.min_cell_voltage +
														 bms_state.min_cell_voltage) * 12 * 6 * 9 / (2 * 10);
	if(msTicks - prechargeTimeout > DEAD_RECKON_TIME ||  sensor_readings.voltage > cutoff_voltage) {
    // After a succificent amount of time has been spent in precharge or voltage
		// reaches threshold, proceed to close high side contactor and prepare to
		// turn on charger
		Board_Pin_Set(PIN_PRECHARGE, 1);
		set_csb_state(CSB_STATE_CHARGE);
	}
}
