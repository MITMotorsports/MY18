#include "main.h"

int main(void) {
  Board_Chip_Init();
  Board_GPIO_Init();
  Board_UART_Init(57600);
  //Board_CAN_Init();
  return 0;
}



