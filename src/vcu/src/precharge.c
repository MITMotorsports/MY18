
#include "precharge.h"
#include "main.h"

void initPrecharge() {
	prechargeStartTime = HAL_GetTick();
}

void loopPrecharge() {
	if (HAL_GetTick() - prechargeStartTime >= PRECHARGE_TOO_LONG_DURATION) {

		changeCarMode(CAR_STATE_CHARGE_FAULT);

	} else if (mc_voltage.busVoltage / DC_BUS_VOLTAGE_SCALE_FACTOR > DC_BUS_VOLTAGE_THRESHOLD) {

		HAL_GPIO_WritePin(CLOSE_CONTACTORS_PORT, CLOSE_CONTACTORS_PIN, GPIO_PIN_SET); // ON
		changeCarMode(CAR_STATE_READY_TO_DRIVE);

	}
}
