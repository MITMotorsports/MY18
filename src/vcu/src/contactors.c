#include "contactors.h"

void openLowSideContactor(void) {
  printf("OPEN LOW SIDE\r\n");
  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_RESET);
}

void closeLowSideContactor(void) {
  printf("CLOSE LOW SIDE\r\n");
  HAL_GPIO_WritePin(GPIO(L_CONTACTOR), GPIO_PIN_SET);
}

void openHighSideContactor(void) {
  printf("OPEN HIGH SIDE\r\n");
  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_RESET);
}

void closeHighSideContactor(void) {
  printf("CLOSE HIGH SIDE\r\n");
  HAL_GPIO_WritePin(GPIO(H_CONTACTOR), GPIO_PIN_SET);
}
