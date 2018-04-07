#ifndef _CARSTATS_H_
#define _CARSTATS_H_

#include "CANlib.h"

#include <stdbool.h>

typedef enum {
    VCU_STATE_ROOT,
    VCU_STATE_LV,
    VCU_STATE_PRECHARGING,
    VCU_STATE_RTD,
    VCU_STATE_DRIVING
} VCU_STATE_T;

typedef struct {
    int battery_voltage;
    int lowest_cell_voltage;
    int max_cell_temp;
    int power;
    bool brake_pressed;
    bool accel_pressed;

    uint32_t last_bms_heartbeat;
    int soc;

    // motor controller torque
    int torque_mc;
    int motor_rpm;
    int front_left_wheel_speed;
    int front_right_wheel_speed;
    int rear_left_wheel_speed;
    int rear_right_wheel_speed;

    VCU_STATE_T vcu_state;

    can0_VcuToDash_T vcu_data;
} carstats_t;

void can_update_carstats(carstats_t *cs);

#endif
