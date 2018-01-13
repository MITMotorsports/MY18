#include "board.h"
#include "../../../lib/lpc11cx4-library/lpc_chip_11cxx_lib/inc/chip.h"
#include "../../../lib/lpc11cx4-library/evt_lib/inc/ltc6804.h"
#include "pins.h"

volatile uint32_t msTicks;

void SysTick_Handler(void) {
	msTicks++;
}

void Board_Chip_Init(void) {
	SysTick_Config(Hertz2Ticks(1000));
}
void Board_GPIO_Init(void) {
    Chip_GPIO_Init(LPC_GPIO);

    //LED1
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED1, IOCON_LED1_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED1, false);

    //LED2
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED2);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED2, IOCON_LED2_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED2, false);   

    //LED3
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED3);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED3, IOCON_LED3_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED3, false);

    //FSAE specific pin intializations   

    //High Side Detect
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_HIGH_SIDE_DETECT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_HIGH_SIDE_DETECT, (IOCON_FUNC0 | IOCON_MODE_INACT));

    //Low Side Detect
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LOW_SIDE_DETECT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LOW_SIDE_DETECT, (IOCON_FUNC0 | IOCON_MODE_INACT));

    //Charge Enable Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_CHARGER_ENABLE);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CHARGER_ENABLE,
            (IOCON_FUNC0 | IOCON_MODE_INACT));

    //Fault Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_BMS_FAULT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_BMS_FAULT,
            (IOCON_FUNC0 | IOCON_MODE_INACT));
}