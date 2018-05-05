#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn =24000000;

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



