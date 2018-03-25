#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include "state.h"

#define SEND_DASH_MSG_WAIT_DURATION 200 // ms

uint32_t lastDashMsgTime;

void handleBrakeThrottleMsg(Frame *msg);

void handleMCVoltageMsg(Frame *msg);

void handleBMSHeartbeatMsg(Frame *msg);

void handleCurrentSensorVoltageMsg(Frame *msg);

void handleCellVoltagesMsg(Frame *msg);

void handleButtonRequest(Frame *msg);

void sendDashMsg();

void sendTorqueCmdMsg(int16_t torque);

void sendMotorOffCmdMsg();

#endif // ifndef __CAN_HANDLES_H
