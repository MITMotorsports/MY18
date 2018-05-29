#ifndef _CARSTATS_H_
#define _CARSTATS_H_

#include "CANlib.h"

#include <stdbool.h>

typedef struct {
    int cs_voltage;  // in dV
    int mc_voltage;  // in dV
    int cs_current;  // in mA
    int max_cell_voltage, min_cell_voltage;
    int max_cell_temp, min_cell_temp;
    int power;
    int current;

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

    can0_VCUHeartbeat_vcu_state_T vcu_state;
    can0_VCUHeartbeat_error_state_T error_state;
    bool estop_hit;
    uint32_t last_vcu_heartbeat;

    int16_t max_igbt_temp;

    can0_VCUErrors_T vcu_errors;
} carstats_t;

int can_update_carstats(carstats_t *cs, can0_ButtonRequest_T *button_request);

#endif
