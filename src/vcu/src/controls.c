#include "controls.h"

static bool enabled = false;
static int16_t torque_command = 0;
static int16_t speed_command = 0;
Controls_Settings_T control_settings = {};
static Launch_Control_State_T lc_state = BEFORE;

static uint32_t get_front_wheel_speed(void);
static bool any_lc_faults();

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
  lc_state = BEFORE;

  set_brake_valve(false);
  lock_brake_valve();
}

bool get_controls_enabled(void) {
  return enabled;
}

void execute_controls(void) {
  static bool up_to_speed = false;

  if (!enabled) return;


  torque_command = get_torque();

  if (control_settings.using_launch_control && lc_state != DONE) {
    // We shouldn't be braking in launch control, so don't worry about regen
    set_brake_valve(false);
    uint32_t front_wheel_speed = get_front_wheel_speed();

    // Mini FSM
    switch (lc_state) {
      case BEFORE:
        sendSpeedCmdMsg(0, 0);

        // Transition
        if (pedalbox_min(accel) > LC_ACCEL_BEGIN) {
          lc_state = SPEEDING_UP;
          printf("[LAUNCH CONTROL] SPEEDING UP STATE ENTERED\r\n");
        }
        break;
      case SPEEDING_UP:
        if (1000 > torque_command) sendSpeedCmdMsg(500, torque_command);
        else sendSpeedCmdMsg(500, 1000);

        // Transition
        if (any_lc_faults()) {
          lc_state = ZERO_TORQUE;
          printf("[LAUNCH CONTROL] ZERO TORQUE STATE ENTERED\r\n");
        } else if (front_wheel_speed > LC_WS_THRESH) {
          lc_state = SPEED_CONTROLLER;
          printf("[LAUNCH CONTROL] SPEED CONTROLLER STATE ENTERED\r\n");
        }
        break;
      case SPEED_CONTROLLER:
        speed_command = get_launch_control_speed(front_wheel_speed);
        sendSpeedCmdMsg(speed_command, torque_command);

        // Transition
        if (any_lc_faults()) {
          lc_state = ZERO_TORQUE;
          printf("[LAUNCH CONTROL] ZERO TORQUE STATE ENTERED\r\n");
        }
      case ZERO_TORQUE:
        sendTorqueCmdMsg(0);

        if (pedalbox_max(accel) < LC_ACCEL_RELEASE) {
          lc_state = DONE;
          printf("[LAUNCH CONTROL] DONE STATE ENTERED\r\n");
        }
        break;
    }

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

  int16_t raw_torque =  MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) / (MAX_ACCEL_VAL - PEDALBOX_ACCEL_RELEASE);
  return raw_torque * control_settings.limp_factor / 100;
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

static int32_t get_launch_control_speed(uint32_t front_wheel_speed) {
uint32_t front_wheel_speedRPM = front_wheel_speed / 1000;
  // Divide 100 because slip ratio is times 100, divide by 100 again because
  // gear ratio is also multiplied by 100
  int32_t target_speed = front_wheel_speedRPM * control_settings.slip_ratio * LC_cGR / (100 * 100);
  return target_speed;
}

static uint32_t get_front_wheel_speed() {
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
  return avg_wheel_speed;
}

static bool any_lc_faults() {
  return pedalbox_min(accel) < LC_ACCEL_BEGIN ||
        pedalbox.brake_2 > LC_BRAKE_BEGIN     ||
        mc_readings.speed > 0; // Backwards is positive
}

Launch_Control_State_T get_lc_state() {
  return lc_state;
}

void reset_lc_state() {
  lc_state = BEFORE;
}
