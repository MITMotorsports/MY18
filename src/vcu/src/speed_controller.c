#include "speed_controller.h"

static SpeedControllerParams speed_controller_params;
static SpeedControllerInternalVars speed_controller_vars;

can0_VCUSpeedControllerInfo_T controller_info = {};
can0_VCUSpeedControllerParams_T controller_params = {};

// PRIVATE FUNCTIONS
static void reset_internal_vars(void);
static void update_error_internal(int32_t actual);

static void reset_internal_vars(void) {
    speed_controller_vars.out_of_input_range_throttled = false;
    speed_controller_vars.out_of_output_range_throttled = false;
    speed_controller_vars.error_update_timed_out = false;
    speed_controller_vars.rpm_setpoint = 0;
    speed_controller_vars.commanded_torque = 0;
    speed_controller_vars.rpm_error = 0;
    speed_controller_vars.deriv_rpm_error = 0;
    speed_controller_vars.last_error_update_timestamp = 0;
    speed_controller_vars.last_rpm_error = 0;
    speed_controller_vars.rpm_error_accumulated = 0;
}

void init_speed_controller_defaults(int32_t max_input_speed, 
    int32_t max_output_torque, uint32_t update_period_millis) {
    
    // Internal controller vars
    speed_controller_vars.enabled = false;
    reset_internal_vars();

    // Parameters
    speed_controller_params.kp_times_1000 = 200;
    speed_controller_params.ki_times_1000 = 0;
    speed_controller_params.kd_times_1000 = 0;
    speed_controller_params.i_windup_max = 10;
    speed_controller_params.i_windup_min = -10;
    speed_controller_params.min_output_value = 0;
    speed_controller_params.max_output_value = max_output_torque;
    speed_controller_params.min_input_value = 0;
    speed_controller_params.max_input_value = max_input_speed;
    // PID loop runs on a fixed update interval to keep i and d updates
    // consistent. Otherwise, integral and derivative term can become unstable.
    speed_controller_params.dt = update_period_millis;

    // TODO: The above constraint must be enforced (enforce dt on the caller probs, may not need to tho)

    // millis of time before controller decides it has not been updated
    // recently enough (done both at a controller internal timestamp
    // and at a actual rpm can msg timestamp level)
    speed_controller_params.error_update_timeout = 50;
}

void enable_speed_controller(void) {
    reset_internal_vars();
    speed_controller_vars.enabled = true;
}

void disable_controls(void) {
    speed_controller_vars.enabled = false;
    reset_internal_vars();
}

void set_speed_controller_setpoint(int32_t rpm) {
    // Input is too low
    if (rpm < speed_controller_params.min_input_value) {
        speed_controller_vars.out_of_input_range_throttled = true;
        speed_controller_vars.rpm_setpoint = speed_controller_params.min_input_value;
        return;
    }

    // Input is too high
    if (rpm > speed_controller_params.max_input_value) {
        speed_controller_vars.out_of_input_range_throttled = true;
        speed_controller_vars.rpm_setpoint = speed_controller_params.max_input_value;
        return;
    }

    speed_controller_vars.rpm_setpoint = rpm;
}

void update_speed_controller_error(int32_t actual_rpm, 
    uint32_t actual_rpm_msg_timestamp) {

    // Check for function call time-outs
    uint32_t func_call_dt = HAL_GetTick() - speed_controller_vars.last_error_update_timestamp;

    if (func_call_dt > speed_controller_params.error_update_timeout) {
        disable_controls();
        speed_controller_vars.error_update_timed_out = true;
        return;
    }

    // Check for CAN based time-outs
    uint32_t can_msg_dt = HAL_GetTick() - actual_rpm_msg_timestamp;

    if (can_msg_dt > speed_controller_params.error_update_timeout) {
        disable_controls();
        speed_controller_vars.error_update_timed_out = true;
        return;
    }

    // TODO: Overhead/underhead counter output so we know if we are not consistent w/ dt
    // Check to see if an update can be performed
    if (HAL_GetTick() - speed_controller_vars.last_error_update_timestamp 
        >= speed_controller_params.dt) {

        update_error_internal(actual_rpm);

        speed_controller_vars.last_error_update_timestamp = HAL_GetTick();
    }
}

static void update_error_internal(int32_t actual) {
    // update from the last step
    speed_controller_vars.last_rpm_error = speed_controller_vars.rpm_error;

    // take error diff
    speed_controller_vars.rpm_error = speed_controller_vars.rpm_setpoint - actual;
    
    // Multiply by 100 for precision, example: 1234 rpm * 1000 / 12ms = 102833, 1 rpm * 1000 / 12ms = 83    
    speed_controller_vars.deriv_rpm_error = 
        (speed_controller_vars.rpm_error 
            - speed_controller_vars.last_rpm_error) * 1000 / speed_controller_params.dt;

    int32_t accum = speed_controller_vars.rpm_error_accumulated 
                        + speed_controller_vars.rpm_error * speed_controller_params.dt;

    // TODO: Double-check dimensions for these
    // account for integral wind-up
    if (accum > speed_controller_params.i_windup_max) {
        accum = speed_controller_params.i_windup_max;
    } else if (accum < speed_controller_params.i_windup_min) {
        accum = speed_controller_params.i_windup_min;
    }

    speed_controller_vars.rpm_error_accumulated = accum;
}

int32_t get_speed_controller_torque_command(void) {
    if (!speed_controller_vars.enabled) {
        return 0;
    }

    // CAREFUL! ADDING MORE PRECISION HERE WILL OVERFLOW THE INT32 FOR THE VALUES THE CONTROLLER IS EXPECTING
    // RUN THIS ON SOME PAPER WITH HIGH AND LOW VALUES
    int32_t torque = (speed_controller_params.kp_times_1000 * speed_controller_vars.rpm_error / 1000)
        + (speed_controller_params.ki_times_1000 * speed_controller_vars.rpm_error_accumulated / 1000)
        + (speed_controller_params.kd_times_1000 * speed_controller_vars.deriv_rpm_error / 1000);

    if (torque < speed_controller_params.min_output_value) {
        speed_controller_vars.out_of_output_range_throttled = true;
        return speed_controller_params.min_output_value;
    }

    if (torque > speed_controller_params.max_output_value) {
        speed_controller_vars.out_of_output_range_throttled = true;
        return speed_controller_params.max_output_value;
    }

    return torque;
}

bool get_speed_controller_enabled(void) {
  return enabled;
}
