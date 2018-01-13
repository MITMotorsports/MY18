#ifndef _BOARD_H_
#define _BOARD_H_

#define Hertz2Ticks(freq) SystemCoreClock / freq
void Board_Chip_Init(void);
void Board_GPIO_Init(void);

#endif