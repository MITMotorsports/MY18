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

// STM DEFINES AND MACROS
#define CANx                            CAN1
#define CANx_CLK_ENABLE() __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define CANx_FORCE_RESET() __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET() __HAL_RCC_CAN1_RELEASE_RESET()

#define CANx_TX_PIN                    GPIO_PIN_9
#define CANx_TX_GPIO_PORT              GPIOB
#define CANx_TX_AF                     GPIO_AF9_CAN1
#define CANx_RX_PIN                    GPIO_PIN_8
#define CANx_RX_GPIO_PORT              GPIOB
#define CANx_RX_AF                     GPIO_AF9_CAN1

#define CANx_RX_IRQn                   CAN1_RX0_IRQn
#define CANx_RX_IRQHandler             CAN1_RX0_IRQHandler

// TODO: Add error checking and have these return aggregate errors.
void handleCAN(CAN_HandleTypeDef *CanHandle);

void handleBrakeThrottleMsg(Frame *msg);
void handleMCVoltageMsg(Frame *msg);
void handleBMSHeartbeatMsg(Frame *msg);
void handleCurrentSensorVoltageMsg(Frame *msg);
void handleCellVoltagesMsg(Frame *msg);
void handleButtonRequest(Frame *msg);

void send_VCUHeartbeat();
void send_VCUErrors();
void send_VCU();

void sendTorqueCmdMsg(int16_t torque);
void sendMotorOffCmdMsg();
void send_mc_fault_clear();

#endif // ifndef __CAN_HANDLES_H
