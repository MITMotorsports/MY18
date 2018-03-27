#include "contactors.h"

void openLowSideContactor(void) {
  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_RESET);
}

void closeLowSideContactor(void) {
  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_SET);
}

void openHighSideContactor(void) {
  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_RESET);
}

void closeHighSideContactor(void) {
  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_SET);
}
