
#include "lv_only.h"
#include "main.h"

void initLVOnly() {
  printf("\r\nCAR STARTED IN LV MODE\r\n");
  changeErrorState(NO_ERROR_NO_ESD_STATE);
  latchingDriverReset = false;
  closingVCUFET       = false;
}

void loopLVOnly() {
  if (buttons.DriverReset && !latchingDriverReset) {
    latchingDriverReset        = true;
    timeSinceLatchSettingStart = HAL_GetTick();

    // Set low now
    HAL_GPIO_WritePin(DRIVER_RESET_TRIGGER_PORT,
                      DRIVER_RESET_TRIGGER_PIN,
                      GPIO_PIN_RESET); // OFF
  } else if (latchingDriverReset) {
    // WE ARE GOING TO LATCH FOR N milliseconds
    if (HAL_GetTick() - timeSinceLatchSettingStart > DRIVER_RESET_LATCHING_TIME) {
      latchingDriverReset = false;

      // SET high again
      HAL_GPIO_WritePin(DRIVER_RESET_TRIGGER_PORT,
                        DRIVER_RESET_TRIGGER_PIN,
                        GPIO_PIN_SET);  // ON

      changeErrorState(NO_ERROR_STATE); // NOW WE ARE CHECKING ESD

      closingVCUFET = true;
    }
  } else if (closingVCUFET) {
    closingVCUFET = false;

    closeLowSideContactor();

    HAL_Delay(CLOSE_VCU_GATE_TIME);             // HOLD OFF TO DO ERROR CHECKING
                                                // FOR SOME TIME

    changeErrorState(NO_ERROR_WITH_TSMS_STATE); // Now checking for TSMS
    changeCarMode(CAR_STATE_PRECHARGING);
  }
}
