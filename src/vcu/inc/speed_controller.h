#ifndef __SPEED_CONTROLLER
#define __SPEED_CONTROLLER

#include "stm32f2xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#include "can_handles.h"

extern can0_VCUSpeedCntrlKpTimes1000_T kp_times_1000_frame;
extern can0_VCUSpeedCntrlKiTimes1000_T ki_times_1000_frame;
extern can0_VCUSpeedCntrlKdTimes1000_T kd_times_1000_frame;
extern can0_VCUSpeedCntrlIWindupMax_T i_windup_max_frame;
extern can0_VCUSpeedCntrlIWindupMin_T i_windup_min_frame;
extern can0_VCUSpeedCntrlMinOutputValue_T min_output_value_frame;
extern can0_VCUSpeedCntrlMaxOutputValue_T max_output_value_frame;
extern can0_VCUSpeedCntrlMinInputValue_T min_input_value_frame;
extern can0_VCUSpeedCntrlMaxInputValue_T max_input_value_frame;
extern can0_VCUSpeedCntrlErrorUpdateTimeout_T error_update_timeout_frame;
extern can0_VCUSpeedCntrlDt_T dt_frame;
extern can0_VCUSpeedCntrlEnabled_T enabled_frame;
extern can0_VCUSpeedCntrlOutOfInputRangeThrottled_T out_of_input_range_throttled_frame;
extern can0_VCUSpeedCntrlOutOfOutputRangeThrottled_T out_of_output_range_throttled_frame;
extern can0_VCUSpeedCntrlErrorUpdateTimedOut_T error_update_timed_out_frame;
extern can0_VCUSpeedCntrlRPMSetpoint_T rpm_setpoint_frame;
extern can0_VCUSpeedCntrlCommandedTorque_T commanded_torque_frame;
extern can0_VCUSpeedCntrlRPMError_T rpm_error_frame;
extern can0_VCUSpeedCntrlLastRPMError_T last_rpm_error_frame;
extern can0_VCUSpeedCntrlDerivRPMError_T deriv_rpm_error_frame;
extern can0_VCUSpeedCntrlRPMErrorAccumulated_T rpm_error_accumulated_frame;
extern can0_VCUSpeedCntrlLastErrorUpdateTimestamp_T last_error_update_timestamp_frame;

// PUBLIC STRUCTS
typedef struct {
	int32_t kp_times_1000;
    int32_t ki_times_1000;
    int32_t kd_times_1000;
    int32_t i_windup_max;
    int32_t i_windup_min;
    int32_t min_output_value;
    int32_t max_output_value;
    int32_t min_input_value;
    int32_t max_input_value;
    int32_t dt;
   	uint32_t error_update_timeout;
} SpeedControllerParams;

typedef struct {
    bool enabled;
    bool out_of_input_range_throttled;
    bool out_of_output_range_throttled;
    bool error_update_timed_out;
    int32_t rpm_setpoint;
    int32_t commanded_torque;
    int32_t rpm_error;
    int32_t last_rpm_error;
    int32_t deriv_rpm_error;
    int32_t rpm_error_accumulated;
    uint32_t last_error_update_timestamp;
} SpeedControllerInternalVars;

// PUBLIC FUNCTIONS
void init_speed_controller_defaults(int32_t max_input_speed, 
    int32_t max_output_torque, uint32_t update_period_millis);
void enable_speed_controller(void);
void disable_speed_controller(void);
void set_speed_controller_setpoint(int32_t rpm);
void update_speed_controller_error(int32_t actual_rpm, 
	uint32_t actual_rpm_msg_timestamp);
int32_t get_speed_controller_torque_command(void);
bool get_speed_controller_enabled(void);
int32_t get_speed_controller_error(void);

#endif // ifndef __SPEED_CONTROLLER
