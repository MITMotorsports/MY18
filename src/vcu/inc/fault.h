#ifndef __FAULT_H
#define __FAULT_H

#include "fault_contactors.h"
#include "fault_gates.h"
#include "fault_heartbeats.h"

void handle_fatal_fault(void);

void handle_test_fault(void);

#endif
