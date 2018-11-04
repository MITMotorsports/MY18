#include "fault_heartbeats.h"

volatile Heartbeats_T heartbeats = {};

const Time_T BMS_HB_MAX_TIME = 1000;
const Time_T FCN_HB_MAX_TIME = 10000;
const Time_T MC_HB_MAX_TIME  = 1000;
const Time_T CS_HB_MAX_TIME  = 1000;


bool check_BMS_heartbeat_bad(void) {
  static bool last_ret = false;
  bool ret = (HAL_GetTick() - heartbeats.bms > BMS_HB_MAX_TIME);

  if (ret != last_ret) {
    last_ret = ret;

    if (ret) {
      printf("[FAULT : HEARTBEATS] BMS HEARTBEAT TIMEOUT.\r\n");
    }
    else {
      printf("[FAULT : HEARTBEATS] BMS HEARTBEAT OK.\r\n");
    }
  }

  return ret;
}

bool check_FCN_heartbeat_bad(void) {
  static bool last_ret = false;
  bool ret = (HAL_GetTick() - heartbeats.fcn > FCN_HB_MAX_TIME);

  if (ret != last_ret) {
    last_ret = ret;

    if (ret) {
      printf("[FAULT : HEARTBEATS] FCN HEARTBEAT TIMEOUT.\r\n");
    }
    else {
      printf("[FAULT : HEARTBEATS] FCN HEARTBEAT OK.\r\n");
    }
  }


  return ret;
}

bool check_MC_heartbeat_bad(void) {
  static bool last_ret = false;
  bool ret =  (HAL_GetTick() - heartbeats.mc > MC_HB_MAX_TIME);

  if (ret != last_ret) {
    last_ret = ret;

    if (ret) {
      printf("[FAULT : HEARTBEATS] MC HEARTBEAT TIMEOUT.\r\n");
    }
    else {
      printf("[FAULT : HEARTBEATS] MC HEARTBEAT OK.\r\n");
    }
  }

  return false;
}

// bool check_CS_heartbeat_bad(void) {
  // return (HAL_GetTick() - heartbeats.current_sensor > CS_HB_MAX_TIME);
// }

// Returns true if there are any heartbeat faults.
bool any_recoverable_heartbeat_faults(void) {
  return check_BMS_heartbeat_bad() ||
         check_FCN_heartbeat_bad() ||
         check_MC_heartbeat_bad();
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
