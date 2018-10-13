#ifndef __CAN_HANDLES_H
#define __CAN_HANDLES_H

#include "stdio.h"
#include "stm32f2xx_hal.h"
#include "CANlib.h"
#include <stdbool.h>
#include <stdint.h>

#include "state.h"
#include "fault.h"
#include "gpio.h"
#include "extern.h"
#include "fault_heartbeats.h"
#include "fault_pedalbox.h"

#define LIMIT(name)                                         \
  static Time_T last_sent = 0;                              \
  if (HAL_GetTick() - last_sent < name ## _period) return;  \
  last_sent = HAL_GetTick();

// TODO: Add error checking and have these return aggregate errors.
void handleCAN(CAN_HandleTypeDef *CanHandle);

void handleBrakeThrottleMsg(Frame *msg);
void handleMCVoltageMsg(Frame *msg);
void handleMCFaultCodesMsg(Frame *msg);
void handleBMSHeartbeatMsg(Frame *msg);
void handleCurrentSensorVoltageMsg(Frame *msg);
void handleCellVoltagesMsg(Frame *msg);
void handleButtonRequest(Frame *msg);
void handleCurrentSensor_Power(Frame *msg);
void handleMCMotor_Position_Info(Frame *msg);
void handleSBG_EKF_Velocity(Frame *msg);
void handleDashRequest(Frame *msg);

void send_VCUHeartbeat(void);
void send_VCUErrors(void);
void send_VCU(void);

void sendTorqueCmdMsg(int16_t torque);
void sendSpeedCmdMsg(int16_t speed, int16_t torque_limit);
void sendMotorOffCmdMsg(void);
void send_mc_fault_clear(void);

void sendVCUSpeedCntrlKpTimes1000Msg(int32_t val);
void sendVCUSpeedCntrlKiTimes1000Msg(int32_t val);
void sendVCUSpeedCntrlKdTimes1000Msg(int32_t val);
void sendVCUSpeedCntrlIWindupMaxMsg(int32_t val);
void sendVCUSpeedCntrlIWindupMinMsg(int32_t val);
void sendVCUSpeedCntrlMinOutputValueMsg(int32_t val);
void sendVCUSpeedCntrlMaxOutputValueMsg(int32_t val);
void sendVCUSpeedCntrlMinInputValueMsg(int32_t val);
void sendVCUSpeedCntrlMaxInputValueMsg(int32_t val);
void sendVCUSpeedCntrlErrorUpdateTimeoutMsg(uint32_t val);
void sendVCUSpeedCntrlDtMsg(int32_t val);
void sendVCUSpeedCntrlEnabledMsg(bool val);
void sendVCUSpeedCntrlOutOfInputRangeThrottledMsg(bool val);
void sendVCUSpeedCntrlOutOfOutputRangeThrottledMsg(bool val);
void sendVCUSpeedCntrlErrorUpdateTimedOutMsg(bool val);
void sendVCUSpeedCntrlRPMSetpointMsg(int32_t val);
void sendVCUSpeedCntrlCommandedTorqueMsg(int32_t val);
void sendVCUSpeedCntrlRPMErrorMsg(int32_t val);
void sendVCUSpeedCntrlLastRPMErrorMsg(int32_t val);
void sendVCUSpeedCntrlDerivRPMErrorMsg(int32_t val);
void sendVCUSpeedCntrlRPMErrorAccumulatedMsg(int32_t val);
void sendVCUSpeedCntrlLastErrorUpdateTimestampMsg(uint32_t val);

#endif // ifndef __CAN_HANDLES_H
