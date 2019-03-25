#ifndef __CONTROLS_PWR_LIM_H
#define __CONTROLS_PWR_LIM_H

#include <stdint.h>

int32_t get_power_limited_torque(int32_t pedal_torque, int32_t tThresh, int32_t Nsteps);

#endif // __CONTROLS_PWR_LIM_H
