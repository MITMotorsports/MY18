#ifndef _CARSTATS_H_
#define _CARSTATS_H_

#include <stdbool.h>

typedef struct {
    int battery_voltage;
    int lowest_cell_voltage;
    int max_cell_temp;
    int power;
    
    bool brake_pressed;
    bool accel_pressed;

    int torque;
    int motor_rpm;
    int front_left_wheel_speed;
    int front_right_wheel_speed;
    int rear_left_wheel_speed;
    int rear_right_wheel_speed;
} carstats_t;

void can_update_carstats(carstats_t *cs);

#endif
