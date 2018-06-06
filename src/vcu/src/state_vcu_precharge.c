#include "state_vcu_precharge.h"
#include "fault.h"

// Choose which bus voltage you want to use:
#define bus_voltage mc_readings.V_bus

static Time_T prechargeTimeout;
static Time_T voltagePrintTimeout;

const Time_T PRINT_VOLTAGE_TIME = 500;
const Time_T DEAD_RECKON_TIME   = 10000;

inline Voltage_T calculate_precharge_target(void) {
  voltages.precharge_target = (voltages.pack * 9) / 10;

  return voltages.precharge_target;
}

void enter_vcu_state_precharge(void) {
  // printf("[TEST] Triggering BEFORE LOW side close action.\r\n");
  // handle_test_fault();

  printf("[VCU FSM : PRECHARGE] ENTERED!\r\n");
  printf("[VCU FSM : PRECHARGE] DEAD RECKONING TIME IS %dms\r\n",
         DEAD_RECKON_TIME);

  // Reset timeout timers to current time.
  prechargeTimeout    = HAL_GetTick();
  voltagePrintTimeout = HAL_GetTick();

  calculate_precharge_target();

  // Write HVDCDC pin high to disable it
  HAL_GPIO_WritePin(GPIO(HVDCDC), GPIO_PIN_SET);
}

void update_vcu_state_precharge(void) {
  closeLowSideContactor();

  // printf("[TEST] Triggering AFTER LOW side close action.\r\n");
  // handle_test_fault();

  // Print the voltage
  if (HAL_GetTick() - voltagePrintTimeout > PRINT_VOLTAGE_TIME) {
    printf("[VCU FSM : PRECHARGE] DC Bus: %ddV / Pack: %ddV\r\n",
           bus_voltage, voltages.precharge_target, voltages.pack);

    voltagePrintTimeout = HAL_GetTick();
  }

  // Dead reckoning with time
  if (HAL_GetTick() - prechargeTimeout > DEAD_RECKON_TIME ||
      bus_voltage > voltages.precharge_target) {
    // printf("[VCU FSM : PRECHARGE] Waiting for second Driver Reset press to close HIGH side.\r\n");
    // while (!buttons.DriverReset) {}
    closeHighSideContactor(); // TODO: What do you, dear reader, think about
                              // contactor atomicity and coupling in
                              // contactors.c?

    // Write HVDCDC pin low to enable it agian
    HAL_GPIO_WritePin(GPIO(HVDCDC), GPIO_PIN_RESET);

    set_vcu_state(VCU_STATE_RTD);
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
  //    set_vcu_state(VCU_STATE_RTD);

  // }
}
