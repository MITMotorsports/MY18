#include "speed_controller.h"

static SpeedControllerParams speed_controller_params;
static SpeedControllerInternalVars speed_controller_vars;

can0_VCUSpeedCntrlKpTimes1000_T kp_times_1000_frame = {};
can0_VCUSpeedCntrlKiTimes1000_T ki_times_1000_frame = {};
can0_VCUSpeedCntrlKdTimes1000_T kd_times_1000_frame = {};
can0_VCUSpeedCntrlIWindupMax_T i_windup_max_frame = {};
can0_VCUSpeedCntrlIWindupMin_T i_windup_min_frame = {};
can0_VCUSpeedCntrlMinOutputValue_T min_output_value_frame = {};
can0_VCUSpeedCntrlMaxOutputValue_T max_output_value_frame = {};
can0_VCUSpeedCntrlMinInputValue_T min_input_value_frame = {};
can0_VCUSpeedCntrlMaxInputValue_T max_input_value_frame = {};
can0_VCUSpeedCntrlErrorUpdateTimeout_T error_update_timeout_frame = {};
can0_VCUSpeedCntrlDt_T dt_frame = {};
can0_VCUSpeedCntrlEnabled_T enabled_frame = {};
can0_VCUSpeedCntrlOutOfInputRangeThrottled_T out_of_input_range_throttled_frame = {};
can0_VCUSpeedCntrlOutOfOutputRangeThrottled_T out_of_output_range_throttled_frame = {};
can0_VCUSpeedCntrlErrorUpdateTimedOut_T error_update_timed_out_frame = {};
can0_VCUSpeedCntrlRPMSetpoint_T rpm_setpoint_frame = {};
can0_VCUSpeedCntrlCommandedTorque_T commanded_torque_frame = {};
can0_VCUSpeedCntrlRPMError_T rpm_error_frame = {};
can0_VCUSpeedCntrlLastRPMError_T last_rpm_error_frame = {};
can0_VCUSpeedCntrlDerivRPMError_T deriv_rpm_error_frame = {};
can0_VCUSpeedCntrlRPMErrorAccumulated_T rpm_error_accumulated_frame = {};
can0_VCUSpeedCntrlLastErrorUpdateTimestamp_T last_error_update_timestamp_frame = {};

// PRIVATE FUNCTIONS
static void reset_internal_vars(void);
static void update_error_internal(int32_t actual);
static void update_can_frames(void);
static void send_speed_controller_can_msgs(void);

static void reset_internal_vars(void) {
    speed_controller_vars.out_of_input_range_throttled = false;
    speed_controller_vars.out_of_output_range_throttled = false;
    speed_controller_vars.error_update_timed_out = false;
    speed_controller_vars.rpm_setpoint = 0;
    speed_controller_vars.commanded_torque = 0;
    speed_controller_vars.rpm_error = 0;
    speed_controller_vars.deriv_rpm_error = 0;
    speed_controller_vars.last_error_update_timestamp = HAL_GetTick();
    speed_controller_vars.last_rpm_error = 0;
    speed_controller_vars.rpm_error_accumulated = 0;
}

void init_speed_controller_defaults(int32_t max_input_speed, 
    int32_t max_output_torque, uint32_t update_period_millis) {
    
    // Internal controller vars
    speed_controller_vars.enabled = false;
    reset_internal_vars();

    // Parameters
    speed_controller_params.kp_times_1000 = 1500;
    speed_controller_params.ki_times_1000 = 10;
    speed_controller_params.kd_times_1000 = 1000;
    speed_controller_params.i_windup_max = 100000;
    speed_controller_params.i_windup_min = -100000;
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

void disable_speed_controller(void) {
    printf("DISABLE SPEED CONTROL!!!!!!!!!!!\r\n");
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

int32_t get_speed_controller_error(void) {
    return speed_controller_vars.rpm_error;
}

int32_t get_speed_controller_accum(void) {
    return speed_controller_vars.rpm_error_accumulated;
}

int32_t get_speed_controller_deriv(void) {
    return speed_controller_vars.deriv_rpm_error;
}

void update_speed_controller_error(int32_t actual_rpm, 
    uint32_t actual_rpm_msg_timestamp) {

    // Check for function call time-outs
    // uint32_t func_call_dt = HAL_GetTick() - speed_controller_vars.last_error_update_timestamp;

    // if (func_call_dt > speed_controller_params.error_update_timeout) {
    //     disable_speed_controller();
    //     speed_controller_vars.error_update_timed_out = true;
    //     return;
    // }

    // Check for CAN based time-outs
    // uint32_t can_msg_dt = HAL_GetTick() - actual_rpm_msg_timestamp;

    // if (can_msg_dt > speed_controller_params.error_update_timeout) {
    //     disable_speed_controller();
    //     speed_controller_vars.error_update_timed_out = true;
    //     return;
    // }

    // TODO: Overhead/underhead counter output so we know if we are not consistent w/ dt
    // Check to see if an update can be performed
    if (HAL_GetTick() - speed_controller_vars.last_error_update_timestamp 
        >= speed_controller_params.dt) {

        update_error_internal(actual_rpm);

        speed_controller_vars.last_error_update_timestamp = HAL_GetTick();
    }

    update_can_frames();
    send_speed_controller_can_msgs();
}

static void update_error_internal(int32_t actual) {
    // update from the last step
    speed_controller_vars.last_rpm_error = speed_controller_vars.rpm_error;

    // take error diff
    speed_controller_vars.rpm_error = speed_controller_vars.rpm_setpoint - (-1.0 * actual); // motor speed is negative
    
    // Multiply by 100 for precision, example: 1234 rpm * 1000 / 12ms = 102833, 1 rpm * 1000 / 12ms = 83    
    speed_controller_vars.deriv_rpm_error = 
        (speed_controller_vars.rpm_error 
            - speed_controller_vars.last_rpm_error) / speed_controller_params.dt;

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
        + (speed_controller_params.ki_times_1000 * speed_controller_vars.rpm_error_accumulated / 10000) // divide by one more here for keeping the rate proper
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

static void update_can_frames(void) {
    kp_times_1000_frame.kp_times_1000 = speed_controller_params.kp_times_1000;
    
    ki_times_1000_frame.ki_times_1000 = speed_controller_params.ki_times_1000;
    
    kd_times_1000_frame.kd_times_1000 = speed_controller_params.kd_times_1000;
    
    i_windup_max_frame.i_windup_max = speed_controller_params.i_windup_max;
    
    i_windup_min_frame.i_windup_min = speed_controller_params.i_windup_min;
    
    min_output_value_frame.min_output_value = speed_controller_params.min_output_value;
    
    max_output_value_frame.max_output_value = speed_controller_params.max_output_value;
    
    min_input_value_frame.min_input_value = speed_controller_params.min_input_value;
    
    max_input_value_frame.max_input_value = speed_controller_params.max_input_value;
    
    dt_frame.dt = speed_controller_params.dt;
    
    error_update_timeout_frame.error_update_timeout = speed_controller_params.error_update_timeout;


    
    enabled_frame.enabled = speed_controller_vars.enabled;
    
    out_of_input_range_throttled_frame.out_of_input_range_throttled = speed_controller_vars.out_of_input_range_throttled;
    
    out_of_output_range_throttled_frame.out_of_output_range_throttled = speed_controller_vars.out_of_output_range_throttled;
    
    error_update_timed_out_frame.error_update_timed_out = speed_controller_vars.error_update_timed_out;
    
    rpm_setpoint_frame.rpm_setpoint = speed_controller_vars.rpm_setpoint;
    
    commanded_torque_frame.commanded_torque = speed_controller_vars.commanded_torque;
    
    rpm_error_frame.rpm_error = speed_controller_vars.rpm_error;
    
    last_rpm_error_frame.last_rpm_error = speed_controller_vars.last_rpm_error;
    
    deriv_rpm_error_frame.deriv_rpm_error = speed_controller_vars.deriv_rpm_error;
    
    rpm_error_accumulated_frame.rpm_error_accumulated = speed_controller_vars.rpm_error_accumulated;
    
    last_error_update_timestamp_frame.last_error_update_timestamp = speed_controller_vars.last_error_update_timestamp;
}

void send_speed_controller_can_msgs(void) {
    sendVCUSpeedCntrlKpTimes1000Msg();
    sendVCUSpeedCntrlKiTimes1000Msg();
    sendVCUSpeedCntrlKdTimes1000Msg();
    sendVCUSpeedCntrlIWindupMaxMsg();
    sendVCUSpeedCntrlIWindupMinMsg();
    sendVCUSpeedCntrlMinOutputValueMsg();
    sendVCUSpeedCntrlMaxOutputValueMsg();
    sendVCUSpeedCntrlMinInputValueMsg();
    sendVCUSpeedCntrlMaxInputValueMsg();
    sendVCUSpeedCntrlErrorUpdateTimeoutMsg();
    sendVCUSpeedCntrlDtMsg();
    sendVCUSpeedCntrlEnabledMsg();
    sendVCUSpeedCntrlOutOfInputRangeThrottledMsg();
    sendVCUSpeedCntrlOutOfOutputRangeThrottledMsg();
    sendVCUSpeedCntrlErrorUpdateTimedOutMsg();
    sendVCUSpeedCntrlRPMSetpointMsg();
    sendVCUSpeedCntrlCommandedTorqueMsg();
    sendVCUSpeedCntrlRPMErrorMsg();
    sendVCUSpeedCntrlLastRPMErrorMsg();
    sendVCUSpeedCntrlDerivRPMErrorMsg();
    sendVCUSpeedCntrlRPMErrorAccumulatedMsg();
    sendVCUSpeedCntrlLastErrorUpdateTimestampMsg();
}

bool get_speed_controller_enabled(void) {
  return speed_controller_vars.enabled;
}
