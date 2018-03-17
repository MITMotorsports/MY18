#ifndef _CARSTATS_H_
#define _CARSTATS_H_

#include "CANLib.h"

#include <stdbool.h>

typedef struct {
    int battery_voltage;
    int lowest_cell_voltage;
    int max_cell_temp;
    int power;
    
    bool brake_pressed;
    bool accel_pressed;

    // motor controller torque
    int torque_mc;
    // torque from can node
    int torque_requested;
    int motor_rpm;
    int front_left_wheel_speed;
    int front_right_wheel_speed;
    int rear_left_wheel_speed;
    int rear_right_wheel_speed;

    can0_VcuToDash_T vcu_data;
} carstats_t;

void can_update_carstats(carstats_t *cs);

#endif
