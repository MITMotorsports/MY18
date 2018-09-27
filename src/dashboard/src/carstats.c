#include "carstats.h"

#include "board.h"
#include "uart.h"

#include <string.h>

Frame frame;

void can_handle_cell_temps(carstats_t *cs) {
    can0_CellTemperatureRange_T msg;
    unpack_can0_CellTemperatureRange(&frame, &msg);

    cs->max_cell_temp = msg.max0;
    cs->min_cell_temp = msg.min;
}

void can_handle_cell_voltages(carstats_t *cs) {
    can0_CellVoltageRange_T msg;
    unpack_can0_CellVoltageRange(&frame, &msg);

    cs->max_cell_voltage = msg.max;
    cs->min_cell_voltage = msg.min;
}

void can_handle_current_sensor_voltage(carstats_t *cs) {
    can0_CurrentSensor_Voltage1_T msg;
    unpack_can0_CurrentSensor_Voltage1(&frame, &msg);

    cs->cs_voltage = msg.result / 100;  // convert from mV to dV
}

void can_handle_current_sensor_current(carstats_t *cs) {
    can0_CurrentSensor_Current_T msg;
    unpack_can0_CurrentSensor_Current(&frame, &msg);

    cs->cs_current = msg.result;  // convert from mA to mA
}

void can_handle_mc_voltage(carstats_t *cs) {
    can0_MCVoltage_T msg;
    unpack_can0_MCVoltage(&frame, &msg);

    cs->mc_voltage = msg.bus - 25;  // convert from dV to dV
}

void can_handle_current_sensor_power(carstats_t *cs) {
    can0_CurrentSensor_Power_T msg;
    unpack_can0_CurrentSensor_Power(&frame, &msg);

    cs->power = msg.result;
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
    cs->estop_hit          = msg.estop_hit;

    cs->last_vcu_heartbeat = msTicks;
}

void can_handle_bms_heartbeat(carstats_t *cs) {
    can0_BMSHeartbeat_T msg;
    unpack_can0_BMSHeartbeat(&frame, &msg);

    cs->last_bms_heartbeat = msTicks;
    cs->soc = msg.soc;
}

void can_handle_mc_temperature1(carstats_t *cs) {
    can0_MCTemperature_T msg;
    unpack_can0_MCTemperature(&frame, &msg);

    int16_t max = msg.module_a_temp;
    if (msg.module_b_temp > max)
        max = msg.module_b_temp;
    if (msg.module_c_temp > max)
        max = msg.module_c_temp;

    cs->max_igbt_temp = max;
}

void can_handle_vcu_errors(carstats_t *cs) {
    can0_VCUErrors_T msg;
    unpack_can0_VCUErrors(&frame, &msg);

    memcpy(&(cs->vcu_errors), &msg, sizeof(msg));
}

void can_handle_button_request(carstats_t *cs) {
    can0_ButtonRequest_T msg;
    unpack_can0_ButtonRequest(&frame, &msg);
    memcpy(&(cs->button_bank), &msg, sizeof(msg));
}

void can_handle_brake_throttle(carstats_t *cs) {
    can0_FrontCanNodeBrakeThrottle_T msg;
    unpack_can0_FrontCanNodeBrakeThrottle(&frame, &msg);

    cs->brake_1 = msg.brake_1;
    cs->brake_2 = msg.brake_2;
}

void can_handle_vcu_controls(carstats_t *cs) {
    unpack_can0_VCUControlsParams(&frame, &cs->vcu_controls);
    cs->vcu_controls_received = true;
}

void can_update_carstats(carstats_t *cs) {
    handle_can_error(Can_RawRead(&frame));

    can0_T msgType;
    msgType = identify_can0(&frame);

    switch (msgType) {
        case can0_FrontCanNodeBrakeThrottle:
            can_handle_brake_throttle(cs);
            break;
        case can0_CellTemperatureRange:
            can_handle_cell_temps(cs);
            break;
        case can0_CellVoltageRange:
            can_handle_cell_voltages(cs);
            break;
        case can0_CurrentSensor_Voltage1:
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
        case can0_MCTemperature:
            can_handle_mc_temperature1(cs);
            break;
        case can0_ButtonRequest:
            can_handle_button_request(cs);
            break;
        case can0_VCUErrors:
            can_handle_vcu_errors(cs);
            break;
        case can0_VCUControlsParams:
            can_handle_vcu_controls(cs);
        default:
            // do nothing
            break;
    }
}
