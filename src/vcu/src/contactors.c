#include "contactors.h"

void openLowSideContactor(void) {
  printf("[CONTACTORS] Commanding LOW side OPEN.\r\n");
  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_RESET);
}

void closeLowSideContactor(void) {
  printf("[CONTACTORS] Commanding LOW side CLOSED.\r\n");
  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_SET);
}

void openHighSideContactor(void) {
  printf("[CONTACTORS] Commanding HIGH side OPEN.\r\n");
  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_RESET);
}

void closeHighSideContactor(void) {
  printf("[CONTACTORS] Commanding HIGH side CLOSED.\r\n");
  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_SET);
}
