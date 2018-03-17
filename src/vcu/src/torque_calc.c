
#include "torque_calc.h"

uint16_t calcTorque(uint16_t accel1, uint16_t accel2) {
	// Get the min value
	uint32_t accel = (accel1 < accel2) ? accel1 : accel2;
	accel = (accel < MIN_SENSOR_VAL) ? MIN_SENSOR_VAL : accel;
	accel = (accel > MAX_SENSOR_VAL) ? MAX_SENSOR_VAL : accel;

	uint16_t torque = MAX_TORQUE * accel / TORQUE_SCALE_FACTOR;
	return torque;
}
