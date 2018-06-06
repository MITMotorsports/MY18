#include "fault_pedalbox.h"

Conflicts_T conflicts        = {};
volatile Pedalbox_T pedalbox = {};


bool any_recoverable_conflict_faults(void) {
  update_conflicts();
  update_implausibility();
  return conflicts.brake_accel;
}

bool any_fatal_conflict_faults(void) {
  update_conflicts();
  update_implausibility();
  return conflicts.accel;
}

void update_conflicts() {
  if (conflicts.brake_accel) {
    // Clear conflict if accel was released.
    // printf("[FAULT : PEDALBOX : BRAKE_ACCEL] Conflict.\r\n");
    if ((pedalbox_min(accel) < PEDALBOX_ACCEL_RELEASE) &&
        (pedalbox.brake_2 < PEDALBOX_BRAKE_BEGIN)) {
      printf("[FAULT : PEDALBOX : BRAKE_ACCEL] Potential release.\r\n");
      conflicts.brake_accel = false;
    }
    else {
      // If conflict not cleared, keep it as is.
      return;
    }
  }

  bool brake_engaged = pedalbox.brake_2 > PEDALBOX_BRAKE_BEGIN;
  bool accel_engaged = pedalbox_min(accel) > PEDALBOX_ACCEL_BEGIN;

  conflicts.brake_accel = brake_engaged && accel_engaged;
}

void update_implausibility() {
  // If there's an implausibility retain it until cleared manually.
  if (conflicts.accel) return;

  bool implausibility = (pedalbox_max(accel) - pedalbox_min(accel)) >
                        PEDALBOX_ACCEL_IMPLAUSIBLE;

  // There is an implausibility if the range of accel meaasurements is above the
  // implausibility threshold.

  if (implausibility) {
    if (conflicts.observed_implausibility) {
      if (HAL_GetTick() - conflicts.last_implausibility >
          IMPLAUSIBILITY_TIMEOUT) {
        conflicts.accel = true;
      }
    }
    else {
      conflicts.last_implausibility = HAL_GetTick();
    }
  }

  conflicts.observed_implausibility = implausibility;
}

int32_t get_pascals(brake_reading) {
  // Real conversion: lpc_voltage = (3.6 * brake_reading / 1024)
  int32_t lpc_voltage = 36 * brake_reading; // Needs to be divided by 10240

  // Real convesrion: sensor_voltage = 1.53  * lpc_voltage
  int32_t sensor_voltage = 153 * lpc_voltage; // Needs to be divded by 100

  // Divide out factors from earlier
  // Real conversion: 1500 / 4 * (sensor_voltage - 0.5)
  int32_t pressure = 1500 * (2 * sensor_voltage - 1) / (2 * 4 * 10240 * 100);

  printf("LPC voltage: %d, sensor voltage: %d, pressure: %d", lpc_voltage, sensor_voltage, pressure);

  return pressure * 689476 / 100; // Convert to pascals
}
