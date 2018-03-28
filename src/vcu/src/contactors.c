#include "contactors.h"

void openLowSideContactor(void) {
  if (READ_PIN(L_CONTACTOR)) printf("[CONTACTORS] LOW side OPEN.\r\n");

  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_RESET);
}

void closeLowSideContactor(void) {
  if (!READ_PIN(L_CONTACTOR)) printf("[CONTACTORS] LOW side CLOSED.\r\n");

  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_SET);
}

void openHighSideContactor(void) {
  if (READ_PIN(H_CONTACTOR)) printf("[CONTACTORS] HIGH side OPEN.\r\n");

  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_RESET);
}

void closeHighSideContactor(void) {
  if (!READ_PIN(H_CONTACTOR)) printf("[CONTACTORS] HIGH side CLOSED.\r\n");

  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_SET);
}
