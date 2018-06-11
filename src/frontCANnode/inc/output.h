#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "timer.h"
#include "types.h"
#include "serial.h"

#include "chip.h"
#include "can.h"

void Output_initialize(void);
void Output_process_output(void);

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (msTicks - last_sent < period) return;                 \
  last_sent = msTicks;

#endif
