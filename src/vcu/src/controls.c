#include "controls.h"

static bool enabled = false;
static int16_t torque_command = 0;
static int16_t speed_command = 0;
Controls_Settings_T control_settings = {};

void enable_controls(void) {
  enabled = true;
  torque_command = 0;
  unlock_brake_valve();
}

void disable_controls(void) {
  enabled = false;
  torque_command = 0;

  set_brake_valve(false);
  lock_brake_valve();
}

bool get_controls_enabled(void) {
  return enabled;
}

void execute_controls(void) {
  if (!enabled) return;

  torque_command = get_torque();

  if (true){ //control_settings.using_launch_control) {
    // We shouldn't be braking in launch control, so don't worry about regen
    set_brake_valve(false);

    speed_command = get_launch_control_speed();

    sendSpeedCmdMsg(speed_command, 0);
  } else {
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
    }

    sendTorqueCmdMsg(torque_command);
  }
}

static int16_t get_torque(void) {
  auto accel = pedalbox_avg(accel);

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
      (get_pascals(pedalbox.FRONT_BRAKE) > RG_FRONT_BRAKE_THRESH) &&
      car_speed_squared > 25) { // Threshold is 5 kph, so 25 (kph)^2
    int32_t kilo_pascals = get_pascals(pedalbox.FRONT_BRAKE) / 1000;
    // Because we already divded by 1000 by using kilo_pascals instead of
    // pascals, we only need to divde by 10^4, not 10^7 for RG_10_7_K
    // And by dividing by 10^3 instead of 10^4, we are multiplying by 10 to get
    // dNm instead of Nm
    regen_torque = RG_10_7_K * kilo_pascals * (100 - control_settings.cBB_ef) /(control_settings.cBB_ef * 1000);
    if (regen_torque > RG_TORQUE_COMMAND_MAX) {
      regen_torque = RG_TORQUE_COMMAND_MAX;
    }
  }

  return -1 * regen_torque;
}

static int32_t get_launch_control_speed() {
  uint32_t left_front_speed;
  uint32_t right_front_speed;

  // If the 32 bit wheel speed is zero, then it's disconnect (in which case we should
  // use the 16 bit one), or the wheels are actually not moving, so the 16 bit one will
  // be zero as well
  if (wheel_speeds.front_left_32b_wheel_speed == 0) {
    left_front_speed = wheel_speeds.front_left_16b_wheel_speed;
  } else {
    left_front_speed = wheel_speeds.front_left_32b_wheel_speed;
  }

  if (wheel_speeds.front_right_32b_wheel_speed == 0) {
    right_front_speed = wheel_speeds.front_right_16b_wheel_speed;
  } else {
    right_front_speed = wheel_speeds.front_right_32b_wheel_speed;
  }

  uint32_t avg_wheel_speed = left_front_speed/2 + right_front_speed/2;
  int32_t target_speed = avg_wheel_speed * LC_10_5_WS_FACTOR / (100000);
  return target_speed;
}
