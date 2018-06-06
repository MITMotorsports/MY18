#ifndef __FAULT_H
#define __FAULT_H

#include "fault_contactors.h"
#include "fault_gates.h"
#include "fault_heartbeats.h"
#include "fault_boards.h"

#include "state.h"

// CONTAINER DEFINITIONS
typedef struct {
  bool gate;
  bool precharge;
  bool contactor;
  bool conflict;
} FATAL_FAULTS_T;

typedef struct {
  bool gate;  // Only includes SHUTDOWN_SENSE(_GATE) AKA ESD
  bool heartbeat;
  bool conflict;
  bool contactor;
} RECOVERABLE_FAULTS_T;


// CONTAINER DECLARATIONS
extern RECOVERABLE_FAULTS_T recoverable_faults;
extern FATAL_FAULTS_T fatal_faults;

// INTERACTION FUNCTIONS
bool update_fatal_faults(void);
bool update_recoverable_faults(void);

void handle_fatal_fault(void);
void handle_recoverable_fault(void);
void handle_test_fault(void);

#endif
