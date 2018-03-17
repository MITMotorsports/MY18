#include "board.h"
#include "config.h"
#include "NHD_US2066.h"
#include "CANlib.h"

#include "led.h"
#include "dispatch.h"

#include <string.h>
#include <math.h>

NHD_US2066_OLED oled;

int main(void) {
    Board_Chip_Init();
    Board_GPIO_Init();
    // need a delay after init or the display
    // will sometimes be messed up
    Delay(100);

    init_can0_dash();

    Pin_Write(PIN_OLED_CS, 0);
    Pin_Write(PIN_OLED_RS, 1);
    Pin_Write(PIN_OLED_SCLK, 1);
    Pin_Write(PIN_OLED_SDI, 1);

    LED_HV_off();
    LED_RTD_off();
    LED_IMD_off();
    LED_AMS_off();

    dispatch_init();

    Delay(100);
    oled_init(&oled, NHD_0420CW_NLINES, NHD_0420CW_NCOLS);
    Delay(100);
    oled_init_commands(&oled);
    Delay(100);

    while (1) {
        dispatch_update();
    }

    return 0;
}
