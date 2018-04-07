// Copied from MY17
// Pin numbers match, so nothing changed

#include "timer.h"

void Timer_Init(void) {
  // Timer Initalization
  Chip_TIMER_Init(LPC_TIMER32_0);
  /* Reset the timer */
  Chip_TIMER_Reset(LPC_TIMER32_0);
  /* Set the prescaler to zero */
  Chip_TIMER_PrescaleSet(LPC_TIMER32_0, 0);
  /* Capture on rising edge and enable capture interrupt.
   * Set the first and third bits of the capture control register to 1 */
  LPC_TIMER32_0->CCR |= 5;

  Chip_TIMER_Init(LPC_TIMER32_1);
  /* Reset the timer */
  Chip_TIMER_Reset(LPC_TIMER32_1);
  /* Set the prescaler to zero */
  Chip_TIMER_PrescaleSet(LPC_TIMER32_1, 0);
  /* Capture on rising edge and enable capture interrupt.
   * Set the first and third bits of the capture control register to 1 */
  LPC_TIMER32_1->CCR |= 5;

  /* Set PIO1_5 to the 32 bit timer capture function for wheel speed sensor 1 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, (IOCON_FUNC2|IOCON_MODE_INACT));
  /* Set PIO1_5 to the 32 bit timer capture function for wheel speed sensor 2 */
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, (IOCON_FUNC3|IOCON_MODE_INACT|IOCON_DIGMODE_EN));
}

void Timer_Start(void) {

  NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);    /* Clear pending interrupt Timer 0*/
  NVIC_EnableIRQ(TIMER_32_0_IRQn);	        /* Enable timer interrupt Timer 0 */

  NVIC_ClearPendingIRQ(TIMER_32_1_IRQn);    /* Clear pending interrupt Timer 1 */
  NVIC_EnableIRQ(TIMER_32_1_IRQn);	        /* Enable timer interrupt Timer 1 */

  // Start the timers
  Chip_TIMER_Enable(LPC_TIMER32_0);
  Chip_TIMER_Enable(LPC_TIMER32_1);
}
