#ifndef TIMER_H
#define TIMER_H

extern volatile uint32_t msTicks;

/**
 * @details intializes timers
 */
void Timer_Init(void);

/**
 * @details starts the timers
 */
void Timer_Start(void);

#endif //TIMER_H
