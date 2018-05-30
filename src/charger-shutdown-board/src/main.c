#include "main.h"

volatile uint32_t msTicks;
const uint32_t    OscRateIn = 24000000;

Frame can_input;

void SysTick_Handler(void) {
  msTicks++;
}


int main(void) {
  SystemCoreClockUpdate();
  if (SysTick_Config(SystemCoreClock / 1000)) while (1);


  Board_GPIO_Init();
  Board_UART_Init(57600);
  Board_Println("Currently running "HASH);
  Board_Println("Flashed by: "AUTHOR);

  init_can0_csb();
  Board_Print("CAN initialized\n");

  //init_lcd();
  //Board_Print("LCD initialized\n");

  init_MCP2307();
  Board_Print("MCP2307 initialized\n");

  init_states();
  Board_Print("States initialized\n");

  while (1) {
    can_receive(&can_input);
    advance_states();
  }
}
