#include "main.h"

volatile uint32_t msTicks;

void SysTick_Handler(void){
	msTicks++;
}

int main(void) {
  SystemCoreClockUpdate();
  Board_GPIO_Init();
  Board_UART_Init(57600);
  Board_Print("H e l l o\n");
  while(1){
	  advance_csb_state();
  }
  
  return 0;
}



