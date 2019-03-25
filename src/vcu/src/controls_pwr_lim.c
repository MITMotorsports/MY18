#include "controls_pwr_lim.h"
#include "controls.h"

const int32_t power_limit = 500; // Watts

static int32_t torque_ramp(int32_t pedal_torque, int32_t tMAX, int32_t tThresh, int32_t Nsteps);

int32_t get_power_limited_torque(int32_t pedal_torque, int32_t tThresh, int32_t Nsteps) {
  static int32_t tCAP = MAX_TORQUE; // Set to max torque so that at first calculated tMAX will always be smaller than this

  if (mc_readings.speed < 0) { // Prevent division by zero, make sure we are spinning (negative is forward)

    int32_t tMAX = power_limit/(abs(mc_readings.speed)*628/6000)*10; // Convert RPM to rad/s with 2pi/60, *10 to dNm

    if (tMAX > MAX_TORQUE) tMAX = MAX_TORQUE; // Cap the maximum tMAX

    // If the wheels slip, calculated tMAX will be higher, but we don't want
    // sudden increases in tMAX, and so it will be capped by the maximum that
    // was previously calculated, will be reset once the car's speed goes back to zero
    if(tMAX <= tCAP) tCAP = tMAX;

    power_lim_settings.tMAX = tMAX;
    power_lim_settings.tCAP = tCAP;

    return torque_ramp(pedal_torque, tMAX, tThresh, Nsteps);
  } else {
    tCAP = MAX_TORQUE; // Reset once wheels stop moving
    power_lim_settings.tCAP = tCAP;
    return pedal_torque;
  }
}

static int32_t torque_ramp(int32_t pedal_torque, int32_t tMAX, int32_t tThresh, int32_t Nsteps) {
  // return pedal_torque;

  static int32_t count = 0;
  count++;
  if(count > Nsteps) return tMAX;

  if(tMAX < tThresh) {
    count = 0;
    return pedal_torque; // Reset ramp once torque is below threshold
  }

  return tThresh + (tMAX-tThresh)*((count-1)/Nsteps); // Return ramp starting from tThresh
}
