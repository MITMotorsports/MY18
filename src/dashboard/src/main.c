#include "board.h"
#include "NHD_US2066.h"

NHD_US2066_OLED oled;

int main(void) {
    Board_Chip_Init();

    Board_GPIO_Init();

    Pin_Write(PIN_OLED_CS, 0);
    Pin_Write(PIN_OLED_RS, 1);
    Pin_Write(PIN_OLED_SCLK, 1);
    Pin_Write(PIN_OLED_SDI, 1);

    Pin_Write(PIN_OLED_BS0, 0);
    Pin_Write(PIN_OLED_BS1, 0);
    Pin_Write(PIN_OLED_BS2, 0);



    Delay(100);
    oled_init(&oled, NHD_0420CW_NLINES, NHD_0420CW_NCOLS);
    Delay(100);

    oled_init_commands(&oled);

    while (1) {
        oled_set_pos(&oled, 0, 0);
        oled_print(&oled, "1)");
        oled_set_pos(&oled, 1, 0);
        oled_print(&oled, "2)");
        oled_set_pos(&oled, 2, 0);
        oled_print(&oled, "3)");
        oled_set_pos(&oled, 3, 0);
        oled_print(&oled, "4)");
        oled_update(&oled);

        Delay(1000);
        oled_set_pos(&oled, 0, 5);
        oled_print(&oled, "something");
        oled_update(&oled);

        Delay(1000);
        oled_set_pos(&oled, 1, 5);
        oled_print(&oled, "foo bar");
        oled_update(&oled);

        Delay(1000);
        oled_set_pos(&oled, 2, 5);
        oled_print(&oled, "!!!!!!!!!!");
        oled_update(&oled);

        Delay(1000);
        oled_set_pos(&oled, 3, 5);
        oled_print(&oled, "the end");
        oled_update(&oled);

        Delay(5000);

        oled_clear(&oled);
        Delay(500);
    }

    return 0;
}
