#include "controls.h"

static bool enabled = false;
static int16_t torque_command = 0;
static int16_t speed_command = 0;
can0_VCUControlsParams_T control_settings = {};

static int16_t limiter(uint16_t threshold, uint16_t absolute, uint16_t min_gain, uint16_t reading);

void init_controls_defaults(void) {
  control_settings.using_regen = false;
  control_settings.regen_bias = 56;
  control_settings.limp_factor = 100;
  control_settings.temp_lim_min_gain = 35;
  control_settings.temp_lim_thresh_temp = 55;
  control_settings.volt_lim_min_gain = 35;
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

  // Extra check to ensure we are only sending regen torque when allowed
  if (torque_command == 0) {
    torque_command = regen_torque;
  } else {
    // Only use limits when we're not doing regen
    int16_t voltage_limited_torque;
    if (control_settings.using_voltage_limiting) {
      voltage_limited_torque = get_voltage_limited_torque(torque_command);
    } else {
      voltage_limited_torque = torque_command;
    }

    int16_t temp_limited_torque;
    if (control_settings.using_temp_limiting) {
      temp_limited_torque = get_temp_limited_torque(torque_command);
    } else {
      temp_limited_torque = torque_command;
    }

    control_settings.torque_temp_limited = temp_limited_torque < torque_command;

    int16_t min_sensor_torque = MIN_DE(voltage_limited_torque, temp_limited_torque);

    int16_t dash_limited_torque = torque_command * control_settings.limp_factor / 100;

    int16_t limited_torque = MIN_DE(min_sensor_torque, dash_limited_torque);

    torque_command = limited_torque;
  }

  sendTorqueCmdMsg(torque_command);
}

static int16_t get_torque(void) {
  auto accel = pedalbox_avg(accel);

  if (accel < PEDALBOX_ACCEL_RELEASE) return 0;

  int16_t torque =  MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) / (MAX_ACCEL_VAL - PEDALBOX_ACCEL_RELEASE);
  return torque;
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

static int16_t get_temp_limited_torque(int16_t pedal_torque) {
  uint16_t temp;
  if (cell_readings.temp_index < TEMP_LOG_LENGTH) {
    temp = cell_readings.temp_log[cell_readings.temp_index];
  } else {
    temp = cell_readings.temp_sum / TEMP_LOG_LENGTH;
  }
  controls_monitoring.filtered_temp = temp;

  if (temp < control_settings.temp_lim_thresh_temp) {
    return pedal_torque;
  }
  int32_t gain;
  if (temp < MAX_TEMP) {
    gain = limiter(control_settings.temp_lim_thresh_temp, MAX_TEMP, control_settings.temp_lim_min_gain, temp);
  } else {
    gain = control_settings.temp_lim_min_gain;
  }
    controls_monitoring.tl_gain = gain;
    return gain * pedal_torque / 100;
}

static int16_t get_voltage_limited_torque(int16_t pedal_torque) {
  int16_t voltage = cs_readings.V_bus / (720); // 72 cells, and we want cV

  if (voltage > control_settings.volt_lim_min_voltage) {
    return pedal_torque;
  }
  int32_t gain;
  if (voltage > MIN_VOLTAGE) {
    gain = limiter(control_settings.volt_lim_min_voltage, MIN_VOLTAGE, control_settings.volt_lim_min_gain, cell_readings.cell_min_cV);
  } else {
    gain = control_settings.volt_lim_min_gain;
  }
  controls_monitoring.vl_gain = gain;
  return gain * pedal_torque / 100;
}

static int16_t limiter(uint16_t threshold, uint16_t absolute, uint16_t min_gain, uint16_t reading) {
  // Explanation:
  // Desired points: LIMITER(threshold) = 100, LIMITER(absolute) = min_gain
  // Slope: (LIMITER(threshold) - LIMITER(absolute)) / (threshold - absolute) = (100 - min_gain) / (threshold - absolute)
  // Intercept: 100 = (100 - min_gain) / (threshold - absolute) * threshold + intercept
  // --> intercept = 100 - (100 - min_gain) / (threshold - absolute) * threshold
  // Add them together and factor out a (threshold - absolute)
  return (100*(threshold - absolute) - (100 - min_gain) * threshold + reading * (100 - min_gain)) / (threshold - absolute);
}
