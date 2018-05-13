// Copied from MY17
// Pin numbers match, so nothing changed

#include "timer.h"

void Timer_Init(void) {
  // Timer Initalization
  Chip_TIMER_Init(LPC_TIMER32_0);
  // Reset the timer
  Chip_TIMER_Reset(LPC_TIMER32_0);
  // Set the prescaler to zero
  Chip_TIMER_PrescaleSet(LPC_TIMER32_0, 0);
  // Capture on rising edge and enable capture interrupt.
  // Set the first and third bits of the capture control register to 1
  LPC_TIMER32_0->CCR |= 5;

  // The rest of the timers are the same (but for the other timers)
  Chip_TIMER_Init(LPC_TIMER32_1);
  Chip_TIMER_Reset(LPC_TIMER32_1);
  Chip_TIMER_PrescaleSet(LPC_TIMER32_1, 0);
  LPC_TIMER32_1->CCR |= 5;

  Chip_TIMER_Init(LPC_TIMER16_0);
  Chip_TIMER_Reset(LPC_TIMER16_0);
  Chip_TIMER_PrescaleSet(LPC_TIMER16_0, 0);
  LPC_TIMER16_0->CCR |= 5;

  Chip_TIMER_Init(LPC_TIMER16_1);
  Chip_TIMER_Reset(LPC_TIMER16_1);
  Chip_TIMER_PrescaleSet(LPC_TIMER16_1, 0);
  LPC_TIMER16_1->CCR |= 5;

  // Configure pins for wheel speed
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC2|IOCON_MODE_INACT));
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, (IOCON_FUNC1|IOCON_MODE_INACT));
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC2|IOCON_MODE_INACT));
  // Pins with dig mode need that set
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, (IOCON_FUNC3|IOCON_MODE_INACT|IOCON_DIGMODE_EN));
}

void Timer_Start(void) {
  // Clear pending interru[ts, then enable timesrs
  NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
  NVIC_EnableIRQ(TIMER_32_0_IRQn);

  NVIC_ClearPendingIRQ(TIMER_32_1_IRQn);
  NVIC_EnableIRQ(TIMER_32_1_IRQn);

  NVIC_ClearPendingIRQ(TIMER_16_0_IRQn);
  NVIC_EnableIRQ(TIMER_16_0_IRQn);

  NVIC_ClearPendingIRQ(TIMER_16_1_IRQn);
  NVIC_EnableIRQ(TIMER_16_1_IRQn);

  // Start the timers
  Chip_TIMER_Enable(LPC_TIMER32_0);
  Chip_TIMER_Enable(LPC_TIMER32_1);
  Chip_TIMER_Enable(LPC_TIMER16_0);
  Chip_TIMER_Enable(LPC_TIMER16_1);
}
