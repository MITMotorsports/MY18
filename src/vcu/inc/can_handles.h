#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "CANlib.h"

#include "state.h"
#include "fault.h"
#include "gpio.h"
#include "extern.h"

#define LIMIT(name)                                         \
  static Time_T last_sent = 0;                              \
  if (HAL_GetTick() - last_sent < name ## _period) return;  \
  last_sent = HAL_GetTick();

// TODO: Add error checking and have these return aggregate errors.
void handleCAN(CAN_HandleTypeDef *CanHandle);

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
