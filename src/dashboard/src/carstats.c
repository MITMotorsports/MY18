#include "carstats.h"

#include "board.h"
#include "uart.h"

#include <string.h>

Frame frame;

void can_handle_front_wheel_speed(carstats_t *cs) {
    can0_FrontCanNodeWheelSpeed_T msg;
    unpack_can0_FrontCanNodeWheelSpeed(&frame, &msg);

    cs->front_left_wheel_speed  = msg.front_left_wheel_speed;
    cs->front_right_wheel_speed = msg.front_right_wheel_speed;
}

//void can_handle_rear_wheel_speed(carstats_t *cs) {
//    can0_RearCanNodeWheelSpeed_T msg;
//    unpack_can0_RearCanNodeWheelSpeed(&frame, &msg);
//
//    cs->rear_left_wheel_speed  = msg.rear_left_wheel_speed;
//    cs->rear_right_wheel_speed = msg.rear_right_wheel_speed;
//}

void can_handle_cell_temps(carstats_t *cs) {
    can0_CellTemperatures_T msg;
    unpack_can0_CellTemperatures(&frame, &msg);

    cs->max_cell_temp = msg.max;
    cs->min_cell_temp = msg.min;
}

void can_handle_cell_voltages(carstats_t *cs) {
    can0_CellVoltages_T msg;
    unpack_can0_CellVoltages(&frame, &msg);

    cs->max_cell_voltage = msg.max;
    cs->min_cell_voltage = msg.min;
}

void can_handle_current_sensor_voltage(carstats_t *cs) {
    can0_CurrentSensor_Voltage_T msg;
    unpack_can0_CurrentSensor_Voltage(&frame, &msg);

    //cs->battery_voltage = msg.dc_bus_voltage;
}

void can_handle_mc_voltage(carstats_t *cs) {
    can0_MCVoltage_T msg;
    unpack_can0_MCVoltage(&frame, &msg);

    cs->battery_voltage = msg.bus;
}

void can_handle_current_sensor_power(carstats_t *cs) {
    can0_CurrentSensor_Power_T msg;
    unpack_can0_CurrentSensor_Power(&frame, &msg);

    cs->power = msg.power;
}

void can_handle_mc_command(carstats_t *cs) {
    can0_MCCommand_T msg;
    unpack_can0_MCCommand(&frame, &msg);

    cs->torque_mc = msg.torque;
    cs->motor_rpm = msg.angular_vel;
}

void can_handle_vcu_heartbeat(carstats_t *cs) {
    can0_VCUHeartbeat_T msg;
    unpack_can0_VCUHeartbeat(&frame, &msg);

    cs->vcu_state          = msg.vcu_state;
    cs->error_state        = msg.error_state;
    cs->last_vcu_heartbeat = msTicks;
}

void can_handle_bms_heartbeat(carstats_t *cs) {
    can0_BMSHeartbeat_T msg;
    unpack_can0_BMSHeartbeat(&frame, &msg);

    cs->last_bms_heartbeat = msTicks;
    cs->soc = msg.soc;
}

void can_handle_mc_temperature1(carstats_t *cs) {
    can0_MCTemperature1_T msg;
    unpack_can0_MCTemperature1(&frame, &msg);

    int16_t max = msg.module_a_temp;
    if (msg.module_b_temp > max)
        max = msg.module_b_temp;
    if (msg.module_c_temp > max)
        max = msg.module_c_temp;

    cs->max_igbt_temp = max;
}

void can_handle_current_sensor_current(carstats_t *cs) {
    can0_CurrentSensor_Current_T msg;
    unpack_can0_CurrentSensor_Current(&frame, &msg);

    cs->current = msg.current;
}

void can_update_carstats(carstats_t *cs, can0_ButtonRequest_T *button_request) {

    handle_can_error(Can_RawRead(&frame));

    can0_T msgType;
    msgType = identify_can0(&frame);
    Board_Print_BLOCKING("ID: ");
    Board_PrintNum(frame.id, 10);
    Board_Println_BLOCKING("");

    switch (msgType) {
        case can0_FrontCanNodeWheelSpeed:
            can_handle_front_wheel_speed(cs);
            break;
        //case can0_RearCanNodeWheelSpeed:
        //    can_handle_rear_wheel_speed(cs);
        //    break;
        case can0_CellTemperatures:
            can_handle_cell_temps(cs);
            break;
        case can0_CellVoltages:
            can_handle_cell_voltages(cs);
            break;
        case can0_CurrentSensor_Voltage:
            can_handle_current_sensor_voltage(cs);
            break;
        case can0_CurrentSensor_Power:
            can_handle_current_sensor_power(cs);
            break;
        case can0_CurrentSensor_Current:
            can_handle_current_sensor_current(cs);
            break;
        case can0_MCCommand:
            can_handle_mc_command(cs);
            break;
        case can0_MCVoltage:
            can_handle_mc_voltage(cs);
            break;
        case can0_VCUHeartbeat:
            can_handle_vcu_heartbeat(cs);
            break;
        case can0_MCTemperature1:
            can_handle_mc_temperature1(cs);
            break;
        case can0_ButtonRequest:
            unpack_can0_ButtonRequest(&frame, button_request);
            break;
        default:

            // do nothing
            break;
    }
}
