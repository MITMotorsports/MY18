#include "state_vcu_precharge.h"
#include "fault.h"

static Time_T prechargeTimeout;
static Time_T voltagePrintTimeout;

const Time_T PRINT_VOLTAGE_TIME = 500;
const Time_T DEAD_RECKON_TIME   = 5000;

inline Voltage_T calculate_precharge_target(void) {
  voltages.precharge_target = (voltages.pack * 9) / 10;

  return voltages.precharge_target;
}

void enter_vcu_state_precharge(void) {
  printf("[VCU FSM : PRECHARGE] ENTERED!\r\n");
  printf("[VCU FSM : PRECHARGE] DEAD RECKONING TIME IS %dms\r\n",
         DEAD_RECKON_TIME);

  // Reset timeout timers to current time.
  prechargeTimeout    = HAL_GetTick();
  voltagePrintTimeout = HAL_GetTick();

  calculate_precharge_target();

  printf("NOT REAL FATAL FAULT\r\n");
  handle_fatal_fault();
}

void update_vcu_state_precharge(void) {
  closeLowSideContactor();

  // Print the voltage
  if (HAL_GetTick() - voltagePrintTimeout > PRINT_VOLTAGE_TIME) {
    printf("[VCU FSM : PRECHARGE] DC Bus: %dV / Pack: %dV\r\n",
           voltages.bus, voltages.precharge_target, voltages.pack);

    voltagePrintTimeout = HAL_GetTick();
  }

  // Dead reckoning with time
  if (HAL_GetTick() - prechargeTimeout > DEAD_RECKON_TIME) {
    printf("GOT TO THE BREAKPOINT AT PRECHARGE END");

    while (1) {}
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
