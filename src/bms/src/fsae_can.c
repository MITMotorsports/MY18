#include "../inc/fsae_can.h"
#include "MY18_Can_Library.h"




static bool isResetting = false;
void Can_Receive(){
	Can_MsgID_T msgType = Can_MsgType();
    if (msgType == Can_No_Msg) {
        // No message, so do nothing this round
        return;
    } else if (msgType == Can_Error_Msg) {
        Can_ErrorID_T err = Can_Error_Read();
        //handle_can_error(err);
        return;
    }

    isResetting = false;

    if (msgType == Can_Unknown_Msg) {
        // TODO use masking instead of this forced read
        Frame f;
        Can_Unknown_Read(&f);
    }  else if (msgType == Can_CurrentSensor_Current_Msg) {
        Can_CurrentSensor_Current_T msg;
        Can_CurrentSensor_Current_Read(&msg);
        //bms_input->pack_status->pack_current_mA = msg.current_mA > 0 ? msg.current_mA : -msg.current_mA;
    } else if (msgType == Can_CurrentSensor_Voltage_Msg) {
        Can_CurrentSensor_Voltage_T msg;
        Can_CurrentSensor_Voltage_Read(&msg);

        //bms_input->pack_status->pack_voltage_mV = msg.voltage_mV > 0 ? msg.voltage_mV : -msg.voltage_mV;
    } else if (msgType == Can_CurrentSensor_Power_Msg) {
        Can_CurrentSensor_Power_T msg;
        Can_CurrentSensor_Power_Read(&msg);

    } else if (msgType == Can_Dash_Request_Msg) {
        Can_Dash_Request_T msg;
        Can_Dash_Request_Read(&msg);
        if (msg.type == CAN_DASH_REQUEST_FAN_ENABLE) {
          //bms_input->fan_override = true;
        } else if (msg.type == CAN_DASH_REQUEST_FAN_DISABLE) {
          //bms_input->fan_override = false;
        }
    } else if(msgType == Can_BMSState_Msg) {
    	Can_BMSState_T msg;
    	Can_BMSState_Read(&msg);
    	//use this msg to change state//
    } else {
        // note other errors
    }
}
