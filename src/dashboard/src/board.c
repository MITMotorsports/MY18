#include "board.h"

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

void SysTick_Handler(void) {
    msTicks++;
}

void Delay(uint32_t dly) {
    uint32_t stop = msTicks + dly;
    while (msTicks < stop);
}

void Board_Chip_Init(void) {
    SysTick_Config(SystemCoreClock / 1000);
}

void Pin_Init(uint8_t port, uint8_t pin) {
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
    Chip_GPIO_SetPinState(LPC_GPIO, port, pin, false); // low
}

void Pin_Write(uint8_t port, uint8_t pin, uint8_t val) {
    Chip_GPIO_SetPinState(LPC_GPIO, port, pin, val);
}

void Board_GPIO_Init(void) {
    Chip_GPIO_Init(LPC_GPIO);

    Pin_Init(PIN_OLED_RS);
    Pin_Init(PIN_OLED_CS);
    Pin_Init(PIN_OLED_SCLK);
    Pin_Init(PIN_OLED_SDI);
    Pin_Init(PIN_OLED_SDO);
    Pin_Init(PIN_OLED_BS0);
    Pin_Init(PIN_OLED_BS1);
    Pin_Init(PIN_OLED_BS2);
}
