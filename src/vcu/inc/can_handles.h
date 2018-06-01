#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "CANlib.h"

#include "state.h"
#include "fault.h"
#include "gpio.h"
#include "extern.h"
#include "fault_heartbeats.h"

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
void handleCurrentSensor_Power(Frame *msg);
void handleMCMotor_Position_Info(Frame *msg);

void send_VCUHeartbeat();
void send_VCUErrors();
void send_VCU();

void sendTorqueCmdMsg(int16_t torque);
void sendMotorOffCmdMsg(void);
void send_mc_fault_clear(void);
void send_PL1_monitoring(void);
void send_PL2_monitoring(void);

#endif // ifndef __CAN_HANDLES_H
