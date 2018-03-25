
#include "contactors.h"
#include "main.h"
#include "state.h"

void initLowSideContactor() {
  GPIO_InitTypeDef gpioinit;

  VCU_GATE_CLK_ENABLE();
  gpioinit.Pin   = VCU_GATE_PIN;
  gpioinit.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(VCU_GATE_PORT, &gpioinit);

  openLowSideContactor();
}

void initHighSideContactor() {
  GPIO_InitTypeDef gpioinit;

  PRECHARGE_GATE_CLK_ENABLE();
  gpioinit.Pin   = PRECHARGE_GATE_PIN;
  gpioinit.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioinit.Pull  = GPIO_PULLUP;
  gpioinit.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(PRECHARGE_GATE_PORT, &gpioinit);

  openHighSideContactor();
}

void openLowSideContactor() {
  HAL_GPIO_WritePin(VCU_GATE_PORT, VCU_GATE_PIN, GPIO_PIN_RESET); // OFF
}

void closeLowSideContactor() {
  HAL_GPIO_WritePin(VCU_GATE_PORT, VCU_GATE_PIN, GPIO_PIN_SET); // ON
}

void openHighSideContactor() {
  HAL_GPIO_WritePin(PRECHARGE_GATE_PORT, PRECHARGE_GATE_PIN, GPIO_PIN_RESET); //
                                                                              // OFF
}

void closeHighSideContactor() {
  HAL_GPIO_WritePin(PRECHARGE_GATE_PORT, PRECHARGE_GATE_PIN, GPIO_PIN_SET); // ON
}
