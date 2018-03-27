#include "state_vcu_precharge.h"
#include "contactors.h"

Time_T prechargeStartTime;
int16_t  targetVoltage;
Time_T showVoltageTime;

#define TIMETIME 5000

int16_t calcTargetVoltage(int16_t packVoltage) {
  return (packVoltage * 9) / 10; // 90% of pack voltage
}

void initPrecharge() {
  printf("[VCU FSM] STARTING PRECHARGE\r\n");
  printf("TIME WILL BE %d\r\n", TIMETIME);
  prechargeStartTime = HAL_GetTick();
  showVoltageTime    = HAL_GetTick();

  if (voltages.packVoltage == 0) {
    printf("[VCU FSM] ERROR! PACK VOLTAGE IS NOT SET\r\n");
    set_vcu_state(VCU_STATE_PRECHARGE_FAULT);
  } else {
    printf("[VCU FSM] PACK VOLTAGE IS %d\r\n", voltages.packVoltage);
    targetVoltage = calcTargetVoltage(voltages.packVoltage);
  }
}

void loopPrecharge() {
  // Timed method for Voltage checking
  // if (HAL_GetTick() - prechargeStartTime >= PRECHARGE_TOO_LONG_DURATION) {

  // printf("\r\n[ERROR]: PRECHARGE TOOK TOO LONG TO GET TO DESIRED DC BUS
  // VOLTAGE\r\n");
  //    set_vcu_state(VCU_STATE_PRECHARGE_FAULT);

  // } else if (mc_voltage.busVoltage / DC_BUS_VOLTAGE_SCALE_FACTOR >=
  // targetVoltage) {

  //    closeHighSideContactor();
  //    set_vcu_state(VCU_STATE_READY_TO_DRIVE);

  // }

  // Print the voltage
  if (HAL_GetTick() - showVoltageTime > 500) {
    printf("[VCU FSM] THE DC BUS VOLTAGE MEASURED IS %d\r\n", voltages.dc_bus_voltage);
    showVoltageTime = HAL_GetTick();
  }

  // Dead reckoning with time
  // TODO: REMOVE THE 240 CHECK
  if (HAL_GetTick() - prechargeStartTime > TIMETIME) {

    printf("ok bois time to close high side\r\n");
    // safety check for integration testing

    // TODO: Remove true, of course.
    if (true || voltages.dc_bus_voltage >= 240) {
      closeHighSideContactor();
      printf("pls go to rtd\r\n");
      set_vcu_state(VCU_STATE_READY_TO_DRIVE);
    } else {
      printf("[VCU FSM] ERROR! PRECHARGE DID NOT BRING DC BUS VOLTAGE TO %d\r\n", 240);
      set_vcu_state(VCU_STATE_PRECHARGE_FAULT);
    }
  }
}
