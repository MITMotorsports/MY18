#include "fault_brakes.h"

static bool locked = true;

void set_brake_valve(bool state) {
  // If the valve is locked force it to turn off on any updates.
  if (locked) state = false;

  HAL_GPIO_WritePin(GPIO(BRAKE_VALVE), state? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void lock_brake_valve(void) {
  locked = true;
  set_brake_valve(false);
}

void unlock_brake_valve(void) {
  locked = false;
}

inline bool get_brake_valve_locked(void) {
  return locked;
}
