#include "state_vcu_precharge.h"
#include "contactors.h"

int16_t calcTargetVoltage(int16_t packVoltage) {
  return (packVoltage * 9) / 10; // 90% of pack voltage
}

void initPrecharge() {
  printf("\r\nSTARTING PRECHARGE\r\n");
  prechargeStartTime = HAL_GetTick();
  showVoltageTime    = HAL_GetTick();

  if (voltages.packVoltage == 0) {
    printf("\r\n[ERROR]: PACK VOLTAGE IS NOT SET\r\n");
    set_vcu_state(VCU_STATE_CHARGE_FAULT);
  } else {
    targetVoltage = calcTargetVoltage(voltages.packVoltage);
  }
}

void loopPrecharge() {
  // Timed method for Voltage checking
  // if (HAL_GetTick() - prechargeStartTime >= PRECHARGE_TOO_LONG_DURATION) {

  // printf("\r\n[ERROR]: PRECHARGE TOOK TOO LONG TO GET TO DESIRED DC BUS
  // VOLTAGE\r\n");
  //    set_vcu_state(VCU_STATE_CHARGE_FAULT);

  // } else if (mc_voltage.busVoltage / DC_BUS_VOLTAGE_SCALE_FACTOR >=
  // targetVoltage) {

  //    closeHighSideContactor();
  //    set_vcu_state(VCU_STATE_READY_TO_DRIVE);

  // }

  // Print the voltage
  if (HAL_GetTick() - showVoltageTime > SHOW_VOLTAGE_FREQUENCY_UPDATE) {
    printf("THE DC BUS VOLTAGE MEASURED IS %d", voltages.dc_bus_voltage);
    showVoltageTime = HAL_GetTick();
  }

  // Dead reckoning with time
  // TODO: REMOVE THE 240 CHECK
  if (HAL_GetTick() - prechargeStartTime >= PRECHARGE_DEAD_RECKONING_TIME) {
    // safety check for integration testing
    if (voltages.dc_bus_voltage >= 240) {
      closeHighSideContactor();
      set_vcu_state(VCU_STATE_READY_TO_DRIVE);
    } else {
      printf("\r\n[ERROR]: PRECHARGE DID NOT BRING DC BUS VOLTAGE TO %d\r\n", 240);
      set_vcu_state(VCU_STATE_CHARGE_FAULT);
    }
  }
}
