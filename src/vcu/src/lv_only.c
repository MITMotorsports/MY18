#include "lv_only.h"

void initLVOnly() {
  printf("\r\nCAR STARTED IN LV MODE\r\n");
  // set_error_state(NO_ERROR_NO_ESD_STATE);
  latchingDriverReset = false;
  closingVCUFET       = false;
}

void loopLVOnly() {
  if (buttons.DriverReset && !latchingDriverReset) {
    latchingDriverReset        = true;
    timeSinceLatchSettingStart = HAL_GetTick();

    // Set low now
    HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_RESET);
  } else if (latchingDriverReset) {
    // WE ARE GOING TO LATCH FOR N milliseconds
    if (HAL_GetTick() - timeSinceLatchSettingStart > DRIVER_RESET_LATCHING_TIME) {
      latchingDriverReset = false;

      // SET high again
      HAL_GPIO_WritePin(GPIO(DRIVER_RST), GPIO_PIN_SET);

      // set_error_state(NO_ERROR_STATE);

      closingVCUFET = true;
    }
  } else if (closingVCUFET) {
    closingVCUFET = false;

    closeLowSideContactor();

    HAL_Delay(CLOSE_VCU_GATE_TIME);             // HOLD OFF TO DO ERROR CHECKING
                                                // FOR SOME TIME

    // set_error_state(NO_ERROR_WITH_TSMS_STATE); // Now checking for TSMS
    // set_vcu_state(VCU_STATE_PRECHARGING);
  }
}
