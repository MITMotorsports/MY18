#include "board.h"
#include "uart.h"
#include "config.h"
#include "CANlib.h"
#include "NHD_US2066.h"

#include "led.h"
#include "dispatch.h"

#include <string.h>
#include <math.h>

int main(void) {
    Board_Chip_Init();
    Board_GPIO_Init();
    Board_UART_Init(UART_BAUD);
    CAN_Init();

    // need a delay after init or the display
    // will sometimes be messed up
    Delay(100);

    Pin_Write(PIN_OLED_CS, 0);
    Pin_Write(PIN_OLED_RS, 1);
    Pin_Write(PIN_OLED_SCLK, 1);
    Pin_Write(PIN_OLED_SDI, 1);

    LED_HV_off();
    LED_RTD_off();
    LED_IMD_off();
    LED_AMS_off();

    dispatch_init();

    Frame f;
    f.id = 0x04;
    f.len = 1;
    f.data[0] = 0;
    f.extended = false;

    while (1) {
        //f.data[0] += 1;
        //handle_can_error(Can_RawWrite(&f));
        //Board_Println("Sent shit");
        //Delay(100);
        //handle_can_error(Can_RawRead(&f));
        //Board_Print_BLOCKING("ID: ");
        //Board_PrintNum(f.id, 10);
        //Board_Println_BLOCKING("");
        //if (f.id == 0x522) {
        //    LED_RTD_on();
        //}

        dispatch_update();
    }

    return 0;
}
