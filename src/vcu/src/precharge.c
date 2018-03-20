
#include "precharge.h"
#include "main.h"

int16_t calcTargetVoltage(int16_t packVoltage) {
	return (packVoltage * 9) / 10; // 90% of pack voltage
}

void initPrecharge() {
	printf("\r\nSTARTING PRECHARGE\r\n");
	prechargeStartTime = HAL_GetTick();
	if (bms_voltage.packVoltage == 0) {
		printf("\r\n[ERROR]: PACK VOLTAGE IS NOT SET\r\n");
		changeCarMode(CAR_STATE_CHARGE_FAULT);
	} else {
		targetVoltage = calcTargetVoltage(bms_voltage.packVoltage);
	}
}

void loopPrecharge() {
	if (HAL_GetTick() - prechargeStartTime >= PRECHARGE_TOO_LONG_DURATION) {

		changeCarMode(CAR_STATE_CHARGE_FAULT);

	} else if (mc_voltage.busVoltage / DC_BUS_VOLTAGE_SCALE_FACTOR >= targetVoltage) {

		HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_SET); // ON
		changeCarMode(CAR_STATE_READY_TO_DRIVE);

	}
}
