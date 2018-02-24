#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

void Output_initialize(Output_T *output);
void Output_process_output(Input_T *input, Output_T *output);

#endif
