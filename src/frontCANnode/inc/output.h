#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "timer.h"
#include "types.h"

void Output_initialize(Output_T *output);
void Output_process_output(Input_T *input, Output_T *output);

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (msTicks - last_sent < period) return;                 \
  last_sent = msTicks;

#endif
