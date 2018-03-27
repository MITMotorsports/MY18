#ifndef __FAULT_CONTACTORS_H
#define __FAULT_CONTACTORS_H

#include "stdio.h"
#include <stdbool.h>

typedef struct {
  bool L_contactor_closed;
  bool H_contactor_closed;
  bool L_contactor_welded;
  bool H_contactor_welded;
} Contactors_T;


extern Contactors_T contactors;


bool any_fatal_contactor_weld_faults(void);

#endif // ifndef __FAULT_CONTACTORS_H
