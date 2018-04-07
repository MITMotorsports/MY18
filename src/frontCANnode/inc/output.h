#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"
#include "serial.h"

#include "chip.h"
#include "can.h"

void Output_initialize(void);
void Output_process_output(void);

#endif
