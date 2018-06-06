#include <stdbool.h>

#include "gpio.h"

void set_brake_valve(bool state);

void lock_brake_valve(void);
void unlock_brake_valve(void);
bool get_brake_valve_locked(void);
