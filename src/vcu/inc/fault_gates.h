#ifndef __FAULT_GATES_H
#define __FAULT_GATES_H

#include "stdio.h"
#include <stdbool.h>

#include "gpio.h"



// CONTAINER DECLARATION
typedef struct {
  bool sdn;
  bool sdn_gate;
  bool bms_gate;
  bool imd_gate;
  bool bpd_gate;
} GateFaults_T;



// GLOBAL CONTAINER DECLARATIONS
extern GateFaults_T gates;



// FUNCTION DECLARATIONS
bool any_all_gate_fault(void);
bool any_fatal_gate_fault(void);
bool any_recoverable_gate_fault(void);

void print_gate_faults(void);


#endif // ifndef __FAULT_GATES_H
