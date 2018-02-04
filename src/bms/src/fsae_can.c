#include "../inc/fsae_can.h"
#include "Can_Library.h"
#include "can.h"
#include "board.h"
#include <stdlib.h>

#define BMS_CAN_TEST_PERIOD 1000
#define BMS_CONTACTOR_WELD_PERIOD 1000

static uint32_t last_bms_can_test_time = 0;
static uint32_t last_bms_contactor_weld_time = 0;

static bool isResetting = false;

static char str[100];


void handle_can_error(Can_ErrorID_T err) {
    if (err == Can_Error_NONE || err == Can_Error_NO_RX) {
        // Neither of these are real errors
        isResetting = false;
        return;
    }
    if (!isResetting) {
        // We have an error, and should start a reset.
        // TODO change behavior depending on error type.
        isResetting = true;
        CAN_ResetPeripheral();
        Board_CAN_Init(500000);
        UNUSED(err);
    }

}

void Can_Receive(BMS_INPUT_T *bms_input){
	Can_MsgID_T msgType = Can_MsgType();
    if (msgType == Can_No_Msg) {
        // No message, so do nothing this round
        return;
    } else if (msgType == Can_Error_Msg) {
        Can_ErrorID_T err = Can_Error_Read();
        handle_can_error(err);
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
        bms_input->pack_status->pack_current_mA = msg.pack_current > 0 ? msg.pack_current : -msg.pack_current;
        itoa(msg.pack_current,str,10);
        Board_Println_BLOCKING(str);
    } else if (msgType == Can_CurrentSensor_Voltage_Msg) {
        Can_CurrentSensor_Voltage_T msg;
        Can_CurrentSensor_Voltage_Read(&msg);
        bms_input->pack_status->pack_voltage_mV = msg.pack_voltage > 0 ? msg.pack_voltage : -msg.pack_voltage;
    } else if (msgType == Can_CurrentSensor_Power_Msg) {
        Can_CurrentSensor_Power_T msg;
        Can_CurrentSensor_Power_Read(&msg);

    } else if (msgType == Can_Dash_Request_Msg) {
        Can_Dash_Request_T msg;
        Can_Dash_Request_Read(&msg);
        if (msg.type == CAN_DASH_REQUEST_FAN_ENABLE) {
          bms_input->fan_override = true;
        } else if (msg.type == CAN_DASH_REQUEST_FAN_DISABLE) {
          bms_input->fan_override = false;
        }
    } else if(msgType == Can_BMS_VCU_State_Msg) {
    	Can_BMS_VCU_State_T msg;
    	Can_BMS_VCU_State_Read(&msg);

        if(msg.state == CAN_BMS_VCU_STATE_DISCHARGE_ENABLE) {
            bms_input->vcu_mode_request = BMS_SSM_MODE_DISCHARGE;
        } else if(msg.state == CAN_BMS_VCU_STATE_DISCHARGE_DISABLE) {
            bms_input->vcu_mode_request = BMS_SSM_MODE_STANDBY;
        } else {
            //VCU sends No_Request;
        }
    	//use this msg to change state//
    } else if (msgType == Can_BMS_CSB_State_Msg){
        Can_BMS_CSB_State_T msg;
        Can_BMS_CSB_State_Read(&msg);

        if(msg.requested_mode == CAN_BMS_CSB_STATE_CHARGE_ENABLE) {
            bms_input->csb_mode_request = BMS_SSM_MODE_CHARGE;
        } else if(msg.requested_mode == CAN_BMS_CSB_STATE_BALANCE_ENABLE) {
            bms_input->csb_mode_request = BMS_SSM_MODE_BALANCE;
        } else if(msg.requested_mode == CAN_BMS_CSB_STATE_CHARGE_DISABLE) {
            bms_input->csb_mode_request = BMS_SSM_MODE_STANDBY;
        } else if(msg.requested_mode == CAN_BMS_CSB_STATE_BALANCE_DISABLE) {
            bms_input->csb_mode_request = BMS_SSM_MODE_STANDBY;
        } else {
            //no request, so don't change csb_mode_request
        }
    } else {
        // note other errors
    }
}

void Can_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output){

    uint32_t msTicks = bms_input->msTicks;
    if((msTicks-last_bms_contactor_weld_time) > BMS_CONTACTOR_WELD_PERIOD) {
        Can_Contactor_Weld_T contactor_weld;
        if(!(bms_input->contactor_weld_two && bms_input->contactor_weld_two)) {
            //Board_Println_BLOCKING("Welded");
            contactor_weld.weld_detect = true;
            Can_Contactor_Weld_Write(&contactor_weld);
            last_bms_can_test_time = msTicks;
        } else if(bms_input->contactor_weld_two && bms_input->contactor_weld_two) {
            //Board_Println_BLOCKING("Not Welded");
            contactor_weld.weld_detect = false;
            Can_Contactor_Weld_Write(&contactor_weld);
            last_bms_can_test_time = msTicks;
        }
    }
}
