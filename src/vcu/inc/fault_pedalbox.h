#ifndef __FAULT_CONFLICTS_H
#define __FAULT_CONFLICTS_H

#include <stdbool.h>

#include "state.h"
#include "extern.h"

#include "CANlib.h"

#define pedalbox_max(name) MAX_DE(pedalbox.name ## _1, pedalbox.name ## _2)
#define pedalbox_min(name) MIN_DE(pedalbox.name ## _1, pedalbox.name ## _2)
#define pedalbox_avg(name) AVG(pedalbox.name ## _1, pedalbox.name ## _2)

// CONSTANTS

// TODO: Make these proportional to MAX_ACCEL_VAL.

// See IC1.13.4 in the rules
#define IMPLAUSIBILITY_TIMEOUT 100

// 10% of 1000, see IC1.13.5 in the rules
#define PEDALBOX_ACCEL_IMPLAUSIBLE 100

// 25% of 1000, see EV2.5 in the rules
#define PEDALBOX_ACCEL_BEGIN 250

// 5% of 1000, see EV2.5.1 in the rules
#define PEDALBOX_ACCEL_RELEASE 50

// When the calipers begin to do anything significant.
#define PEDALBOX_BRAKE_BEGIN 160
#define PEDALBOX_BRAKE_RTD   275


typedef struct {
  bool   brake_accel;
  bool   accel;
  bool   observed_implausibility;
  Time_T last_implausibility;

  can0_FrontCanNodeBrakeThrottle_T fcn;
  bool any_errs;
} Conflicts_T;

typedef struct {
  int16_t accel_1;
  int16_t accel_2;
  int16_t brake_1;
  int16_t brake_2;
} Pedalbox_T;


extern Conflicts_T conflicts;
extern volatile Pedalbox_T pedalbox;


bool any_fatal_conflict_faults(void);
bool any_recoverable_conflict_faults(void);
int32_t get_pascals(int16_t brake_reading);

#endif // ifndef __FAULT_CONFLICTS_H
