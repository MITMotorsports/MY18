#include "board.h"
#include "config.h"
#include "NHD_US2066.h"
#include "CANlib.h"

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



    Delay(100);
    oled_init(&oled, NHD_0420CW_NLINES, NHD_0420CW_NCOLS);
    Delay(100);

    oled_init_commands(&oled);
    oled_print(&oled, "Testing CAN");
    oled_update(&oled);

    Frame frame;
    memset(&frame, 0, sizeof(frame));
    int counter = 0;
    while (1) {
        can0_FrontCanNodeWheelSpeed_T msg;
        msg.front_right_wheel_speed = 420;
        msg.front_left_wheel_speed = 21;

        pack_can0_FrontCanNodeWheelSpeed(&msg, &frame);
        can0_FrontCanNodeWheelSpeed_Write(&msg);

        //can0_T msgType;
        // identify_can0(&frame);
        //Can_RawRead(&frame);

        oled_set_pos(&oled, 1, 0);
        oled_print(&oled, "the ID ");
        oled_print_num(&oled, frame.id);
        oled_update(&oled);

        Delay(50);
    }

    while (1) {
        char str[20] = {'\0'};
        int i, j;
        oled_set_pos(&oled, 0, 0);
        oled_print(&oled, "   Starting Unity");

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
