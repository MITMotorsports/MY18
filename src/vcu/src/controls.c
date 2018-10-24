#include "controls.h"

static bool enabled = false;
static int32_t torque_command = 0;
static int32_t speed_command = 0;
can0_VCUControlsParams_T control_settings = {};

static int32_t hinge_limiter(int32_t x, int32_t m, int32_t e, int32_t c);

// PRIVATE FUNCTIONS
static int32_t get_torque(void);
static int32_t get_regen_torque(void);
static int32_t get_temp_limited_torque(int32_t pedal_torque);
static int32_t get_voltage_limited_torque(int32_t pedal_torque);

void init_controls_defaults(void) {
  control_settings.using_regen = false;
  control_settings.regen_bias = 57;
  control_settings.limp_factor = 100;
  control_settings.temp_lim_min_gain = 25;
  control_settings.temp_lim_thresh_temp = 50;
  control_settings.volt_lim_min_gain = 0;
  control_settings.volt_lim_min_voltage = 300;
  control_settings.torque_temp_limited = false;
}

void enable_controls(void) {
  enabled = true;
  torque_command = 0;
  speed_command = 0;

  unlock_brake_valve();
}

void disable_controls(void) {
  enabled = false;
  torque_command = 0;
  speed_command = 0;

  set_brake_valve(false);
  lock_brake_valve();
}

bool get_controls_enabled(void) {
  return enabled;
}

void execute_controls(void) {
  if (!enabled) return;

  torque_command = get_torque();
  controls_monitoring.raw_torque = torque_command;

  // Control regen brake valve:
  bool brake_valve_state = control_settings.using_regen && get_pascals(pedalbox.REAR_BRAKE) < RG_REAR_BRAKE_THRESH;
  set_brake_valve(brake_valve_state);

  int32_t regen_torque;
  if (brake_valve_state) {
    regen_torque = get_regen_torque();
  } else {
    regen_torque = 0;
  }

  // Calculate commands to set gain values
  (void) get_voltage_limited_torque(torque_command);
  (void) get_temp_limited_torque(torque_command);

  // Extra check to ensure we are only sending regen torque when allowed
  if (torque_command == 0) {
    torque_command = regen_torque;
  }
  else {
    // Only use limits when we're not doing regen
    int32_t voltage_limited_torque = get_voltage_limited_torque(torque_command);
    // static uint32_t last_vt = 0;
    // if (HAL_GetTick() - last_vt > 10) {
    //   printf("VT: %d\r\n", voltage_limited_torque);
    //
    //   last_vt = HAL_GetTick();
    // }
    if (!control_settings.using_voltage_limiting) voltage_limited_torque = torque_command;

    int32_t temp_limited_torque = get_temp_limited_torque(torque_command);
    // static uint32_t last_tt = 0;
    // if (HAL_GetTick() - last_tt > 10) {
    //   printf("TT: %d\r\n", temp_limited_torque);
    //
    //   last_tt = HAL_GetTick();
    // }
    if (!control_settings.using_temp_limiting) temp_limited_torque = torque_command;

    control_settings.torque_temp_limited = temp_limited_torque < torque_command;

    int32_t min_sensor_torque;
    if (voltage_limited_torque < temp_limited_torque) {
      min_sensor_torque = voltage_limited_torque;
    }
    else {
      min_sensor_torque = temp_limited_torque;
    }

    int32_t dash_limited_torque = torque_command * control_settings.limp_factor / 100;

    int32_t limited_torque;
    if (dash_limited_torque < min_sensor_torque) {
      limited_torque = dash_limited_torque;
    }
    else {
      limited_torque = min_sensor_torque;
    }

    torque_command = limited_torque;
  }

  sendTorqueCmdMsg(torque_command);
}

static int32_t get_torque(void) {
  int32_t accel = pedalbox_avg(accel);

  if (accel < PEDALBOX_ACCEL_RELEASE) return 0;

  return MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) / (MAX_ACCEL_VAL - PEDALBOX_ACCEL_RELEASE);
}

static int32_t get_regen_torque() {
  int32_t regen_torque = 0;

  // IMU velocity component unit conversion:
  // Starting units: 100 * m/s, ending units: kph
  // (100 m/s) * (1 km/1000 m) * (3600 s/1 hr) = 360 kph
  // So multiply by 360 to get kph --> but this is squared, so multiply by 129600
  int32_t imu_vel_north_squared = imu_velocity.north * imu_velocity.north;
  int32_t imu_vel_east_squared = imu_velocity.east * imu_velocity.east;
  int32_t car_speed_squared = 129600 * (imu_vel_north_squared + imu_vel_east_squared);

  if ((mc_readings.speed * -1 > RG_MOTOR_SPEED_THRESH) &&
      (cs_readings.V_bus < RG_BATTERY_VOLTAGE_MAX_THRESH) &&
      (get_pascals(pedalbox.FRONT_BRAKE) > RG_FRONT_BRAKE_THRESH)) {
      // car_speed_squared > RG_CAR_SPEED_THRESH * RG_CAR_SPEED_THRESH) { // Threshold is 5 kph, so 25 (kph)^2
    int32_t kilo_pascals = get_pascals(pedalbox.FRONT_BRAKE) / 1000;
    // Because we already divded by 1000 by using kilo_pascals instead of
    // pascals, we only need to divde by 10^4, not 10^7 for RG_10_7_K
    // And by dividing by 10^3 instead of 10^4, we are multiplying by 10 to get
    // dNm instead of Nm
    regen_torque = RG_10_7_K * kilo_pascals * (100 - control_settings.regen_bias) /(control_settings.regen_bias * 1000);
    if (regen_torque > RG_TORQUE_COMMAND_MAX) {
      regen_torque = RG_TORQUE_COMMAND_MAX;
    }
  }

  // Regen is negative torque, and we've calculated a positive number so far
  return -1 * regen_torque;
}

static int32_t get_temp_limited_torque(int32_t pedal_torque) {
  uint32_t temp_sum = 0;
  for (uint32_t i = 0; i < TEMP_LOG_LENGTH; i++) {
    temp_sum += cell_readings.temp_log[i];
  }

  // For higher accuracy (to centi-Celsius) multiply by 10 before dividing
  uint32_t temp_cC = temp_sum * 10 / TEMP_LOG_LENGTH;
  controls_monitoring.filtered_temp = temp_cC;

  // Thresh was in degrees, so multiply it by 100
  int32_t thresh_cC = control_settings.temp_lim_thresh_temp * 100;

  // static uint32_t lastt = 0;
  // if (HAL_GetTick() - lastt > 100) {
  //   printf("Filtered Temp: %d\tTemp threshold: %d\r\n", temp_cC, thresh_cC);
  //
  //   lastt = HAL_GetTick();
  // }

  int32_t gain = hinge_limiter(temp_cC, control_settings.temp_lim_min_gain, thresh_cC, MAX_TEMP);
  controls_monitoring.tl_gain = gain;

  return gain * pedal_torque / 100;
}

static int32_t get_voltage_limited_torque(int32_t pedal_torque) {
  // We want cs_readings.V_bus/72 - 0.1 because of empirical differences
  // We also want centivolts instead of milivolts, so this gives us:
  // (cs_readings.V_bus/72)/10 - 1/10 = (cs_readings.V_bus - 72)/720
  int32_t cell_voltage = (cs_readings.V_bus - 72) / 720;
  controls_monitoring.voltage_used = cell_voltage;

  // static uint32_t lastt = 0;
  // if (HAL_GetTick() - lastt > 100) {
  //   printf("Voltage: %d\tVoltage threshold: %d\r\n", cell_voltage, control_settings.volt_lim_min_voltage);
  //
  //   lastt = HAL_GetTick();
  // }

  int32_t gain = hinge_limiter(cell_voltage, control_settings.volt_lim_min_gain, control_settings.volt_lim_min_voltage, MIN_VOLTAGE);

  controls_monitoring.vl_gain = gain;
  return gain * pedal_torque / 100;
}


// Returns the output of a linear hinge.
// It is a continuous function.
// `m` is the minimum output of this function.
// `c` is the `x` threshold above which the function returns `m`.
// for `m` < `x` < `c` the output is linearly decreasing.
int32_t positive_hinge(int32_t x, int32_t m, int32_t c) {
  if (x < 0) return 100;
  if (x > c) return m;

  return (x * (m - 100) / c) + 100;
}


// Returns the output of a bidirectional linear hinge limiter.
int32_t hinge_limiter(int32_t x, int32_t m, int32_t e, int32_t c) {
  if (c > e) return positive_hinge(x - e, m, c - e);
  else       return positive_hinge(e - x, m, e - c);
}
