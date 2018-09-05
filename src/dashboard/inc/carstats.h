#ifndef _CARSTATS_H_
#define _CARSTATS_H_

#include "CANlib.h"

#include <stdbool.h>
#include <stdint.h>

#include "button_listener.h"

#define LEN(x)  (sizeof(x) / sizeof((x)[0])
#define CONSTRAIN(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : x))
#define LOOPOVER(x, min, max) (((x) < (min)) ? (max) : (((x) > (max)) ? (min) : x))

typedef struct {
    button_state_t left;
    button_state_t right;
    button_state_t A;
    button_state_t B;
} Buttons_T;

typedef struct {
    // Values obseverd by dashboard
    int cs_voltage;  // in dV
    int mc_voltage;  // in dV
    int cs_current;  // in mA
    int max_cell_voltage, min_cell_voltage;
    int max_cell_temp, min_cell_temp;
    int power;
    int current;

    bool brake_pressed;
    int brake_1;
    int brake_2;
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

    can0_ButtonRequest_T button_bank;

    can0_VCUHeartbeat_vcu_state_T vcu_state;
    can0_VCUHeartbeat_error_state_T error_state;
    bool estop_hit;
    uint32_t last_vcu_heartbeat;

    int16_t max_igbt_temp;

    can0_VCUErrors_T vcu_errors;

    // Controls values
    can0_DashRequest_T controls;
    can0_VCUControlsParams_T vcu_controls;

    bool vcu_controls_received;

    Buttons_T buttons;
} carstats_t;

void can_update_carstats(carstats_t *cs);

#endif
