#include "fault_heartbeats.h"

volatile Heartbeats_T heartbeats = {};

const Time_T BMS_HB_MAX_TIME = 1000;
const Time_T FCN_HB_MAX_TIME = 10000;


// Returns true if there are any heartbeat faults.
bool any_recoverable_heartbeat_faults(void) {
  if (HAL_GetTick() - heartbeats.bms > BMS_HB_MAX_TIME) {
    printf("[FAULT : HEARTBEATS] BMS HEARTBEAT TIMEOUT.\r\n");
    return true;
  }

  if (HAL_GetTick() - heartbeats.fcn > FCN_HB_MAX_TIME) {
    printf("[FAULT : HEARTBEATS] FCN HEARTBEAT TIMEOUT.\r\n");
    return true;
  }

  return false;
}

// void printHeartbeatFailures(void) {
//   // printf("LAST BMS: %llu, NOW: %llu", heartbeats.bms,
//   // HAL_GetTick());
//   // if (HAL_GetTick() - heartbeats.bms >=
//   // CAN_BMS_HEARTBEAT_FAULT_DURATION) {
//   //     printf("\r\nBMS HEARTBEAT WAS NOT RECEIVED IN TIMELY FASHION\r\n");
//   // }
//   // if (HAL_GetTick() - heartbeats.frontCanNode >
//   // CAN_FRONT_CAN_NODE_HEARTBEAT_FAULT_DURATION) {
//   //     printf("\r\nFRONT CAN NODE HEARTBEAT WAS NOT RECEIVED IN TIMELY
//   // FASHION\r\n");
//   // }
// }
