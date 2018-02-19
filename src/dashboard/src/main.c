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

    init_can0_dash();

    Pin_Write(PIN_OLED_CS, 0);
    Pin_Write(PIN_OLED_RS, 1);
    Pin_Write(PIN_OLED_SCLK, 1);
    Pin_Write(PIN_OLED_SDI, 1);

    Pin_Write(PIN_OLED_BS0, 0);
    Pin_Write(PIN_OLED_BS1, 0);
    Pin_Write(PIN_OLED_BS2, 0);


    LED_HV_off();
    LED_RTD_off();
    LED_IMD_off();
    LED_AMS_off();

    dispatch_init();

    while (1) {
        dispatch_update();
        Delay(1);
    }


    Delay(100);
    oled_init(&oled, NHD_0420CW_NLINES, NHD_0420CW_NCOLS);
    Delay(100);

    oled_init_commands(&oled);
    oled_print(&oled, "Testing CAN");
    oled_update(&oled);

    Frame frame;
    memset(&frame, 0, sizeof(frame));
    int counter = 0;


    oled_clear(&oled);
    oled_set_double_height_mode(&oled, NHD_US2066_DOUBLE_HEIGHT_MIDDLE); 

    oled_set_pos(&oled, 0, 0);
    oled_print(&oled, "  Big Text !");
    oled_print_char(&oled, CHAR_ARROW_UP);
    oled_set_pos(&oled, 1, 0);
    oled_print(&oled, "BMS FAULT");

    oled_update(&oled);

    int n = 0;
    int r = 2;

    oled_clear(&oled);
    oled_set_double_height_mode(&oled, NHD_US2066_DOUBLE_HEIGHT_NONE);
    int buttonMs = 0;
    while (1) {
        oled_set_pos(&oled, 1, 0);
        oled_print(&oled, "Button 1: ");
        int num = Pin_Read(PIN_BUTTON1) ? 1 : 0;
        oled_print_num(&oled, num);

        oled_set_pos(&oled, 2, 0);
        oled_print(&oled, "Button 2: ");
        if (Pin_Read(PIN_BUTTON2)) {
            
        }
        oled_print_num(&oled, num);

        oled_update(&oled);

        Delay(50);
    }

    while (1) {
        if (n % 6 < 3) {
            oled_clearline(&oled, 1);
        } else {
            oled_set_pos(&oled, 1, 0);
            oled_print(&oled, "BMS FAULT");
        }

        oled_clearline(&oled, 0);
        oled_set_pos(&oled, 0, 5);
        oled_print_num(&oled, n++);
        oled_print(&oled, "KPH");
        oled_update(&oled);

        r = abs((r * 48373) % 947185);
        oled_set_pos(&oled, 2, 0);
        oled_rprint_num_pad(&oled, r % 300, 1);
        oled_rprint(&oled, "V");

        char arr[4] = {CHAR_FORWARD_SLASH,
                    CHAR_PIPE,
                    CHAR_BACKWARD_SLASH,
                    CHAR_MINUS};

        oled_set_pos(&oled, 2, 2);
        int j = 0;
        for (; j < 10; j++) {
            oled_print_char(&oled, arr[n % 4]);
        }

        Delay(100);
    }

    oled_set_pos(&oled, 0, 0);
    oled_print(&oled, "Testing CAN");

    while (1) {
        char str[20] = {'\0'};
        int i, j;
        oled_set_pos(&oled, 0, 0);
        oled_print(&oled, "      Loading");

        for (i = 0; i < 20; i++) {
            for (j = 0; j < 5; j++) {
                str[i] = 0xD4 - j;

                //oled_set_pos(&oled, 1, 0);
                //oled_print(&oled, str);
                //oled_update(&oled);
                //Delay(50);
                if (j == 0)
                    str[i] = '-';
                else if (j == 1)
                    str[i] = '/';
                else if (j == 2)
                    str[i] = 0xFE;
                else if (j == 3)
                    str[i] = 0xFB;
                else if (j == 4)
                    str[i] = 0x1F;

                oled_set_pos(&oled, 1, 0);
                oled_print(&oled, str);
                oled_update(&oled);
                Delay(50);
            }
        }
        Delay(3000);
        oled_clear(&oled);
    }

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
