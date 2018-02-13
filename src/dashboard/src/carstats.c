#include "carstats.h"
#include "CANLib.h"

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

void can_handle_bms_cell_temps(carstats_t *cs) {
    can0_BMSCellTemps_T msg;
    unpack_can0_BMSCellTemps(&frame, &msg);

    cs->max_cell_temp = msg.max_cell_temp;
}

void can_handle_bms_pack_status(carstats_t *cs) {
    can0_BmsPackStatus_T msg;
    unpack_can0_BmsPackStatus(&frame, &msg);

    cs->battery_voltage     = msg.pack_voltage;
    cs->lowest_cell_voltage = msg.min_cell_voltage;
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
        case can0_BMSCellTemps:
            can_handle_bms_cell_temps(cs);
            break;
        case can0_BmsPackStatus:
            can_handle_bms_pack_status(cs);
            break;
        default:
            // do nothing
            break;
    }
}
