#include "state_vcu_lv.h"

void initLVOnly() {
  printf("[VCU FSM] CAR STARTED IN LV MODE\r\n");
  // set_error_state(NO_ERROR_NO_ESD_STATE);
  latchingDriverReset = false;
  closingVCUFET       = false;
}


bool pp = true;

void loopLVOnly() {
  if (not_yet_master_rst) return;
  if (pp && buttons.DriverReset) {
    printf("DriverReset Pressed. Will force release\r\n");
    pp = false;
    // buttons.DriverReset = false;
  }
  if (buttons.DriverReset) {
    closingVCUFET = true;
  }
  // if (buttons.DriverReset && !latchingDriverReset) {
  //   buttons.DriverReset = false; // HACK
  //   latchingDriverReset        = true;
  //   timeSinceLatchSettingStart = HAL_GetTick();
  //
  //   // Set low now
  //   // HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_RESET);
  //   printf("driver reset writ low. faketh\r\n");
  // }
  // else if (latchingDriverReset) {
  //   // WE ARE GOING TO LATCH FOR N milliseconds
  //   if (HAL_GetTick() - timeSinceLatchSettingStart > 10) {
  //     latchingDriverReset = false;
  //
  //     // SET high again
  //     // HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_SET);
  //     printf("driver reset writ high again. fake news\r\n");
  //
  //     // set_error_state(NO_ERROR_STATE);
  //
  //     closingVCUFET = true;
  //   }
  // }
  if (closingVCUFET) {
    closingVCUFET = false;

    closeLowSideContactor();

    HAL_Delay(CLOSE_VCU_GATE_TIME);             // HOLD OFF TO DO ERROR CHECKING
                                                // FOR SOME TIME

    set_vcu_state(VCU_STATE_PRECHARGING);
  }
}
