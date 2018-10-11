#ifndef __SPEED_CONTROLLER
#define __SPEED_CONTROLLER

#include "stm32f2xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#include "can_handles.h"
#include "fault_pedalbox.h"
#include "fault_brakes.h"

// TODO: Extract MAX_TORQUE from RMS EEPROM (over CAN?)
#define MAX_TORQUE_SPEED_CONTROLLER  2400 // unit: dNm
#define MAX_ACCEL_VAL_SPEED_CONTROLLER 1000
#define MIN_ACCEL_VAL_SPEED_CONTROLLER 0

extern can0_VCUSpeedControllerInfo_T controller_info;
extern can0_VCUSpeedControllerParams_T controller_params;

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
void send_speed_controller_info_can_msg(void);
void send_speed_controller_params_can_msg(void);

#endif // ifndef __SPEED_CONTROLLER
