#ifndef __BRAKE_THROTTLE_H
#define __BRAKE_THROTTLE_H

#include "state.h"
#include <stdbool.h>
#include "stdio.h"

// See IC1.13.4 in the rules
#define IMPLAUSIBILITY_REPORT_MS 100

// 10% of 1000, see IC1.13.5 in the rules
#define IMPLAUSIBILITY_THROTTLE_TRAVEL 100

// 25% of 1000, see EV2.5 in the rules
#define CONFLICT_BEGIN_THROTTLE_TRAVEL 250

// 5% of 1000, see EV2.5.1 in the rules
#define CONFLICT_END_THROTTLE_TRAVEL 50

#define CONFLICT_BRAKE_RAW 600

void update_brake_throttle_conflict(Pedalbox_T   brake_throttle,
                                    Conflicts_T *implaus_conflict);
void update_implausibility(Pedalbox_T   brake_throttle,
                           Conflicts_T *implaus_conflict,
                           uint32_t     msg_ticks);

#endif // ifndef __BRAKE_THROTTLE_H
