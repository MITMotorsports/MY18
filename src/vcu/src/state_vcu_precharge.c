#include "state_vcu_precharge.h"

int16_t targetVoltage;
Time_T  prechargeTimeout;
Time_T  voltagePrintTimeout;

const Time_T PRINT_VOLTAGE_TIME = 500;
const Time_T DEAD_RECKON_TIME   = 5000;

int16_t calcTargetVoltage(int16_t pack) {
  return (pack * 9) / 10; // 90% of pack voltage
}

void initPrecharge() {
  printf("[VCU FSM : PRECHARGE] ENTERED!\r\n");
  printf("[VCU FSM : PRECHARGE] DEAD RECKONING TIME IS %dms\r\n",
         DEAD_RECKON_TIME);

  // Reset timeout timers to current time.
  prechargeTimeout    = HAL_GetTick();
  voltagePrintTimeout = HAL_GetTick();

  closeLowSideContactor();
}

void loopPrecharge() {
  // Print the voltage
  if (HAL_GetTick() - voltagePrintTimeout > PRINT_VOLTAGE_TIME) {
    printf("[VCU FSM : PRECHARGE] DC Bus: %dV / Pack: %dV\r\n",
           voltages.bus, voltages.pack);

    voltagePrintTimeout = HAL_GetTick();
  }

  // Dead reckoning with time
  if (HAL_GetTick() - prechargeTimeout > DEAD_RECKON_TIME) {
    closeHighSideContactor(); // TODO: What do you, dear reader, think about
                              // contactor atomicity and coupling in
                              // contactors.c?

    set_vcu_state(VCU_STATE_READY_TO_DRIVE);
    return;
  }

  // Timed method for Voltage checking
  // if (HAL_GetTick() - prechargeTimeout >= PRECHARGE_TOO_LONG_DURATION) {

  // printf("\r\n[ERROR]: PRECHARGE TOOK TOO LONG TO GET TO DESIRED DC BUS
  // VOLTAGE\r\n");
  //    set_vcu_state(VCU_STATE_PRECHARGE_FAULT);

  // } else if (mc_voltage.busVoltage / DC_BUS_VOLTAGE_SCALE_FACTOR >=
  // targetVoltage) {

  //    closeHighSideContactor();
  //    set_vcu_state(VCU_STATE_READY_TO_DRIVE);

  // }
}
