#ifndef __FAULT_CONFLICTS_H
#define __FAULT_CONFLICTS_H

#include <stdbool.h>

#include "state.h"
#include "extern.h"


// CONSTANTS
// See IC1.13.4 in the rules
#define IMPLAUSIBILITY_REPORT_MS 100

// 10% of 1000, see IC1.13.5 in the rules
#define IMPLAUSIBILITY_THROTTLE_TRAVEL 100

// 25% of 1000, see EV2.5 in the rules
#define CONFLICT_BEGIN_THROTTLE_TRAVEL 250

// 5% of 1000, see EV2.5.1 in the rules
#define CONFLICT_END_THROTTLE_TRAVEL 50

#define CONFLICT_BRAKE_RAW 600


typedef struct {
  bool   has_brake_throttle_conflict;
  bool   has_pedalbox_conflict;
  bool   observed_implausibility;
  bool   actual_implausibility;
  Time_T implausibility_ticks;
} Conflicts_T;


extern Conflicts_T conflicts;


bool any_fatal_conflict_faults(void);

#endif // ifndef __FAULT_CONFLICTS_H
