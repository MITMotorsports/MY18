#ifndef _CAN_HANDLES_H
#define _CAN_HANDLES_H

#include <stdbool.h>
#include "CANlib.h"
#include "state.h"

void can_receive(Frame *input);

void handle_can_error(Can_ErrorID_T error);

void handle_ChargerStatus1(Frame *input);

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (msTicks - last_sent < period) return;                 \
  last_sent = msTicks;

extern volatile uint32_t msTicks;

#endif // _CAN_HANDLES_H
