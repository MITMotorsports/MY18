#include "carstats.h"

#include <string.h>

Frame frame;

void can_handle_front_wheel_speed(carstats_t *cs) {
    can0_FrontCanNodeWheelSpeed_T msg;
    unpack_can0_FrontCanNodeWheelSpeed(&frame, &msg);

    cs->front_left_wheel_speed  = msg.front_left_wheel_speed;
    cs->front_right_wheel_speed = msg.front_right_wheel_speed;
}

void can_handle_rear_wheel_speed(carstats_t *cs) {
    can0_RearCanNodeWheelSpeed_T msg;
    unpack_can0_RearCanNodeWheelSpeed(&frame, &msg);

    cs->rear_left_wheel_speed  = msg.rear_left_wheel_speed;
    cs->rear_right_wheel_speed = msg.rear_right_wheel_speed;
}

void can_handle_cell_temps(carstats_t *cs) {
    can0_CellTemperatures_T msg;
    unpack_can0_CellTemperatures(&frame, &msg);

    cs->max_cell_temp = msg.max;
}

void can_handle_cell_voltages(carstats_t *cs) {
    can0_CellVoltages_T msg;
    unpack_can0_CellVoltages(&frame, &msg);

    cs->lowest_cell_voltage = msg.min;
}

void can_handle_current_sensor_voltage(carstats_t *cs) {
    can0_CurrentSensor_Voltage_T msg;
    unpack_can0_CurrentSensor_Voltage(&frame, &msg);

    cs->battery_voltage = msg.pack_voltage;
}

void can_handle_current_sensor_power(carstats_t *cs) {
    can0_CurrentSensor_Power_T msg;
    unpack_can0_CurrentSensor_Power(&frame, &msg);

    cs->power = msg.power;
}

void can_handle_mc_command(carstats_t *cs) {
    can0_MC_Command_T msg;
    unpack_can0_MC_Command(&frame, &msg);

    cs->torque_mc = msg.torque;
    cs->motor_rpm = msg.speed;
}

void can_handle_vcu_to_dash(carstats_t *cs) {
    can0_VcuToDash_T msg;
    unpack_can0_VcuToDash(&frame, &msg);

    memcpy(&(cs->vcu_data), &msg, sizeof(msg));
}

void can_update_carstats(carstats_t *cs) {

    can0_T msgType;
    msgType = identify_can0(&frame);

    switch (msgType) {
        case can0_FrontCanNodeWheelSpeed:
            can_handle_front_wheel_speed(cs);
            break;
        case can0_RearCanNodeWheelSpeed:
            can_handle_rear_wheel_speed(cs);
            break;
        case can0_CellTemperatures:
            can_handle_cell_temps(cs);
            break;
        case can0_CellVoltages:
            can_handle_cell_voltages(cs);
            break;
        case can0_CurrentSensor_Voltage:
            can_handle_current_sensor_voltage(cs);
        case can0_CurrentSensor_Power:
            can_handle_current_sensor_power(cs);
        case can0_MC_Command:
            can_handle_mc_command(cs);
        case can0_VcuToDash:
            can_handle_vcu_to_dash(cs);
        default:
            // do nothing
            break;
    }
}
