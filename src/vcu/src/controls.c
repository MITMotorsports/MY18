#include "controls.h"

static bool enabled = false;
static int32_t torque_command = 0;
static int32_t speed_command = 0;
static uint32_t last_slip_ctrl_exec = 0;
static uint32_t test_time = 0;

// Ring buffer used to add delay to a set of input signal
// values for the LC speed calculations
static RingBuffer32 lc_speed_buf;
static int32_t __lc_speed_buf[N_RING_BUFFER_SLOTS];

can0_VCUControlsParams_T control_settings = {};
can0_VCUControlsParamsLC_T lc_settings = {};
can0_DashSpeedCntrlRPMSetpoint_T rpm_setpoint = {};

static Launch_Control_State_T lc_state = BEFORE;

uint32_t last_time_step = 0;

uint32_t get_front_wheel_speed(void);
static bool any_lc_faults();

static int32_t hinge_limiter(int32_t x, int32_t m, int32_t e, int32_t c);

// PRIVATE FUNCTIONS
static int32_t get_torque(void);
static int32_t get_regen_torque(void);
static int32_t get_launch_control_speed(uint32_t front_wheel_speed);
static int32_t get_temp_limited_torque(int32_t pedal_torque);
static int32_t get_voltage_limited_torque(int32_t pedal_torque);

void init_controls_defaults(void) {
  RingBuffer32_init(&lc_speed_buf, __lc_speed_buf, N_RING_BUFFER_SLOTS);

  control_settings.using_regen = false;
  control_settings.regen_bias = 57;
  control_settings.limp_factor = 100;
  control_settings.temp_lim_min_gain = 25;
  control_settings.temp_lim_thresh_temp = 50;
  control_settings.volt_lim_min_gain = 0;
  control_settings.volt_lim_min_voltage = 300;
  control_settings.torque_temp_limited = false;

  lc_settings.using_launch_ctrl = false;
  lc_settings.launch_ctrl_slip_ratio = LC_DEFAULT_SLIP_RATIO;
  lc_settings.speeding_up_torque = LC_DEFAULT_SPEEDING_UP_TORQUE;
  lc_settings.speeding_up_speed = LC_DEFAULT_SPEEDING_UP_SPEED;
  lc_settings.ws_thresh = LC_DEFAULT_WS_THRESH;

  rpm_setpoint.rpm_setpoint = 0;

  init_speed_controller_defaults(MAX_INPUT_SPEED, 
    MAX_TORQUE, SPEED_CONTROLLER_UPDATE_PERIOD_MS);
}

void enable_controls(void) {
  RingBuffer32_clear(&lc_speed_buf);

  enabled = true;
  torque_command = 0;
  speed_command = 0;
  lc_state = BEFORE;

  unlock_brake_valve();
}

void disable_controls(void) {
  enabled = false;
  torque_command = 0;
  speed_command = 0;
  lc_state = DONE;

  set_brake_valve(false);
  lock_brake_valve();

  RingBuffer32_clear(&lc_speed_buf);
}

bool get_controls_enabled(void) {
  return enabled;
}

void execute_controls(void) {
  if (!enabled) return;
  bool launch_ctrl_entered = false;

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
  } else { // Only use limits and launch control when we're not doing regen
    // Even if we are doing launch control, calculate limits for logging also so that we are not doing
    // launch control when we're supposed to be limiting

    int32_t voltage_limited_torque = get_voltage_limited_torque(torque_command);
   
    if (!control_settings.using_voltage_limiting) voltage_limited_torque = torque_command;

    int32_t temp_limited_torque = get_temp_limited_torque(torque_command);

    if (!control_settings.using_temp_limiting) temp_limited_torque = torque_command;

    uint32_t torque_temp_limited = temp_limited_torque < torque_command;
    // Whether torque is temp limited needs to be sent out over CAN so the dash knows not to allow regen
    control_settings.torque_temp_limited = torque_temp_limited;

    int32_t min_sensor_torque;
    if (voltage_limited_torque < temp_limited_torque) {
      min_sensor_torque = voltage_limited_torque;
    } else {
      min_sensor_torque = temp_limited_torque;
    }

    int32_t dash_limited_torque = torque_command * control_settings.limp_factor / 100;

    int32_t limited_torque;
    if (dash_limited_torque < min_sensor_torque) {
      limited_torque = dash_limited_torque;
    } else {
      limited_torque = min_sensor_torque;
    }

    torque_command = limited_torque;
    bool torque_limited = limited_torque < controls_monitoring.raw_torque;

    if (launch_ctrl_entered = (lc_settings.using_launch_ctrl && lc_state != DONE && !torque_limited)) {
      // We shouldn't be braking in launch control, so reset regen valve
      set_brake_valve(false);
      uint32_t front_wheel_speed = get_front_wheel_speed();

      // Run the slip control loop on a slower interval than the PID loop so that 
      // PID loop can sample the desired speed more quickly
      if (HAL_GetTick() - last_slip_ctrl_exec > SLIP_CONTROLLER_UPDATE_PERIOD_MS) {
          // Wait until the buffer is filled
          if (RingBuffer32_free(&lc_speed_buf) <= 0) {
            // Read the oldest value in the buffer as the current speed command
            
            speed_command = RingBuffer32_readByte(&lc_speed_buf);
          } 

          // Write the newest value in the buffer for a later read
          RingBuffer32_writeByte(&lc_speed_buf, get_launch_control_speed(front_wheel_speed));

          last_slip_ctrl_exec = HAL_GetTick();
      }

      // if (HAL_GetTick() - test_time > 100) {
      //     printf("SPEED_CMD: %d, BUF FREE SPACE: %d, LC SPEED: %d, N_SLOTS: %d\r\n", 
      //       speed_command, RingBuffer32_free(&lc_speed_buf), get_launch_control_speed(front_wheel_speed), N_RING_BUFFER_SLOTS);
      //     test_time = HAL_GetTick();
      // }

      // Mini FSM
      if (any_lc_faults() && lc_state != DONE && lc_state != BEFORE) {
            lc_state = ZERO_TORQUE;
            printf("[LAUNCH CONTROL] ZERO TORQUE STATE ENTERED\r\n");
      }
      switch (lc_state) {
        case BEFORE:
          sendTorqueCmdMsg(0);
          disable_speed_controller();

          // Transition
          if (pedalbox_min(accel) > LC_ACCEL_BEGIN) {
            lc_state = SPEEDING_UP;
            printf("[LAUNCH CONTROL] SPEEDING UP STATE ENTERED\r\n");
          }
          break;
        case SPEEDING_UP:
           // Command max(torque_command, lc_settings.speeding_up_torque)
          // if (torque_command < lc_settings.speeding_up_torque) sendSpeedCmdMsg(lc_settings.speeding_up_speed, torque_command);
          // else sendSpeedCmdMsg(lc_settings.speeding_up_speed, lc_settings.speeding_up_torque);
          sendTorqueCmdMsg(lc_settings.speeding_up_torque);

          // Transition
          if (front_wheel_speed > lc_settings.ws_thresh * 1000) { // 
            lc_state = SPEED_CONTROLLER;
            enable_speed_controller();
            printf("[LAUNCH CONTROL] SPEED CONTROLLER STATE ENTERED\r\n");
          }
          break;
        case SPEED_CONTROLLER:

          set_speed_controller_setpoint(speed_command); // RPM

          // Update the internal speed controller with the new speed value
          // TODO: replace HAL_GetTick with the timestamp of the message
          update_speed_controller_error(mc_readings.speed, HAL_GetTick());
                           
          int32_t speedControlTorqueOutput = get_speed_controller_torque_command();
          if (speedControlTorqueOutput > torque_command) {
            speedControlTorqueOutput = torque_command;
          }

          if (HAL_GetTick() - last_time_step > 50) {
            //printf("[SC] SETPOINT: %d, ERR: %d, TORQUE: %d, ACCUM: %d, DERIV: %d\r\n",
            //  speed_command, get_speed_controller_error(), speedControlTorqueOutput,
            //  get_speed_controller_accum(), get_speed_controller_deriv());

            last_time_step = HAL_GetTick();
          }

          sendTorqueCmdMsg(speedControlTorqueOutput);

          break;
        case ZERO_TORQUE:
          sendTorqueCmdMsg(0);
          disable_speed_controller();

          if (pedalbox_max(accel) < LC_ACCEL_RELEASE) {
            lc_state = DONE;
            printf("[LAUNCH CONTROL] DONE STATE ENTERED\r\n");
          }
          break;
        default:
          sendTorqueCmdMsg(0);
          disable_speed_controller();

          printf("ERROR: NO STATE!\r\n");
          break;
      }
    }
  }
  if (!launch_ctrl_entered) {
    // In LC we already sent our command above
    sendTorqueCmdMsg(torque_command);
  }
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

  return -1 * regen_torque;
}

static int32_t get_launch_control_speed(uint32_t front_wheel_speed) {
uint32_t front_wheel_speedRPM = front_wheel_speed / 1000;
  // Divide 100 because slip ratio is times 100, divide by 100 again because
  // gear ratio is also multiplied by 100
  int32_t target_speed = front_wheel_speedRPM * lc_settings.launch_ctrl_slip_ratio * LC_GR / (100 * 100);
  return target_speed;
}

uint32_t get_front_wheel_speed() {
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
  return left_front_speed;
}

static bool any_lc_faults(void) {
  if (pedalbox_min(accel) < LC_ACCEL_BEGIN) {
    printf("[LAUNCH CONTROL ERROR] Accel min (%d) too low\r\n", pedalbox_min(accel));
    return true;
  } else if (pedalbox.brake_2 > LC_BRAKE_BEGIN) {
    // In this test, we only need to check brake_2 because it is the only brake line
    // that is indicative of driver braking intent. The other brake is for regen.
    printf("[LAUNCH CONTROL ERROR] Brake (%d) too high\r\n", pedalbox.brake_2);
    return true;
  } 

  // else if (mc_readings.speed > LC_BACKWARDS_CUTOFF) {
  //   printf("[LAUNCH CONTROL ERROR] MC reading (%d) is great than cutoff (%d)\r\n", mc_readings.speed, LC_BACKWARDS_CUTOFF);
  //   return true;
  // }
    return false;
}

void set_lc_state_before(void) {
  lc_state = BEFORE;
}

void set_lc_zero_torque(void) {
  lc_state = ZERO_TORQUE;
}

Launch_Control_State_T get_lc_state() {
  return lc_state;
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
