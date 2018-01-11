#include "input.h"
#include "adc.h"

#include <Can_Library.h>

#define ADC_UPDATE_PERIOD_MS 10

void update_adc(Input_T *input);
void update_can(Input_T *input);

void can_process_rear_wheel_speed(Input_T *input);
void can_process_vcu_dash(Input_T *input);
void can_process_bms_soc(Input_T *input);

// TODO Add wheel speed stuff

void Input_initialize(Input_T *input) {
  input->adc->left_throttle_pot = 0;
  input->adc->right_throttle_pot = 0;
  input->adc->front_brake_pressure = 0;
  input->adc->rear_brake_pressure = 0;

  input->can_input->rear_wheel_speed_msg->rear_right_wheel_speed = 0;
  input->can_input->rear_wheel_speed_msg->rear_left_wheel_speed = 0;

  input->can_input->vcu_dash_msg->rtd_light_on = false;
  input->can_input->vcu_dash_msg->ams_light_on = false;
  input->can_input->vcu_dash_msg->imd_light_on = false;
  input->can_input->vcu_dash_msg->hv_light_on = false;
  input->can_input->vcu_dash_msg->traction_control = false;
  input->can_input->vcu_dash_msg->limp_mode_on = false;
  input->can_input->vcu_dash_msg->lv_warning_on = false;
  input->can_input->vcu_dash_msg->active_aero_on = false;
  input->can_input->vcu_dash_msg->regen_on = false;
  input->can_input->vcu_dash_msg->shutdown_esd_drain_open = false;
  input->can_input->vcu_dash_msg->shutdown_bms_open = false;
  input->can_input->vcu_dash_msg->shutdown_imd_open = false;
  input->can_input->vcu_dash_msg->shutdown_bspd_open = false;
  input->can_input->vcu_dash_msg->shutdown_vcu_open = false;
  input->can_input->vcu_dash_msg->shutdown_precharge_open = false;
  input->can_input->vcu_dash_msg->shutdown_master_reset_open = false;
  input->can_input->vcu_dash_msg->shutdown_driver_reset_open = false;
  input->can_input->vcu_dash_msg->heartbeat_front_can_node_dead = false;
  input->can_input->vcu_dash_msg->heartbeat_rear_can_node_dead = false;
  input->can_input->vcu_dash_msg->heartbeat_bms_dead = false;
  input->can_input->vcu_dash_msg->heartbeat_dash_dead = false;
  input->can_input->vcu_dash_msg->heartbeat_mc_dead = false;
  input->can_input->vcu_dash_msg->heartbeat_current_sensor_dead = false;
  input->can_input->vcu_dash_msg->tsms_off = false;
  input->can_input->vcu_dash_msg->reset_latch_open = false;
  input->can_input->vcu_dash_msg->precharge_running = false;
  input->can_input->vcu_dash_msg->master_reset_not_initialized = false;
  input->can_input->vcu_dash_msg->driver_reset_not_initialized = false;
  input->can_input->vcu_dash_msg->lv_battery_voltage = 0;
  input->can_input->vcu_dash_msg->limp_state = CAN_VCUTODASH_LIMP_NORMAL;
}

void Input_fill_input(Input_T *input) {
  update_adc(input);
  update_can(input);
}

// Copied from MY17
void update_adc(Input_T *input) {
  ADC_Input_T *adc = input->adc;
  uint32_t next_updated = adc->last_updated + ADC_UPDATE_PERIOD_MS;

  if (next_updated < input->msTicks) {
    adc->left_throttle_pot = ADC_Read(LEFT_THROTTLE_POT_CHANNEL);
    adc->right_throttle_pot = ADC_Read(RIGHT_THROTTLE_POT_CHANNEL);
    adc->front_brake_pressure = ADC_Read(FRONT_BRAKE_CHANNEL);
    adc->rear_brake_pressure = ADC_Read(REAR_BRAKE_CHANNEL);
    adc->last_updated = input->msTicks;
  }
}

void update_can(Input_T *input) {
  Can_MsgID_T msgID = Can_MsgType();
  switch(msgID) {
    case Can_RearCanNodeWheelSpeed_Msg:
      can_process_rear_wheel_speed(input);
      break;
    case Can_VcuToDash_Msg:
      can_process_vcu_dash(input);
      break;
    case Can_BMS_SOC_Msg:
      can_process_bms_soc(input);
      break;
    default:
      break;
  }
}
