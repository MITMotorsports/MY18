#include "fault_precharge.h"


bool any_fatal_precharge_fault(void) {
  return false;
}


bool check_pack_voltage(void) {
  // Make sure that we are aware of the pack voltage by now.
  if (voltages.pack == 0) {
    printf("[FAULT : PRECHARGE] ERROR! PACK VOLTAGE IS WRONG\r\n");
  }
  // else {
  //   printf("[FAULT : PRECHARGE] PACK VOLTAGE IS %d\r\n", voltages.pack);
  // }
  return true;
}
// static PRECHARGE_FAULT_T latest_fault = precharge_fault_none;
//
// PRECHARGE_FAULT_T raise_precharge_fault(PRECHARGE_FAULT_T cause) {
//   if (cause == precharge_fault_none) {
//     printf("[FAULT : PRECHARGE] Attempted to raise a precharge_fault_none."
//            "Use clear_precharge_fault to do that.\r\n");
//
//     return latest_fault;
//   }
//
// }
//
// void clear_precharge_fault(void) {
//   printf("[FAULT : PRECHARGE] Clearing any fault.\r\n");
//
// }
