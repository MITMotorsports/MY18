#include "NHD_US2066.h"
#include "board.h"

#include <stdlib.h>
#include <string.h>

#define __pos2idx(___oled, ___line, ___col) (___line*___oled->ncols + ___col)

#define NHD_0420CW_NLINES 4

void _OLED_send_serial(unsigned char c, unsigned char temp) {
    unsigned char i;
    uint8_t val;
    // some random magic I have no idea
    for (i = 0; i < 8; i++) {
        Pin_Write(PIN_OLED_SCLK, 0);
        val = ((temp & 0x80)>>7 == 1) ? 1 : 0;
        Pin_Write(PIN_OLED_SDI, val);
        temp = temp << 1;
        Pin_Write(PIN_OLED_SCLK, 1);
    }

    // lower 4 bits
    for (i = 0; i < 4; i++) {
        Pin_Write(PIN_OLED_SCLK, 0);
        Pin_Write(PIN_OLED_SDI, c & 0x01);
        c = c >> 1;
        Pin_Write(PIN_OLED_SCLK, 1);
    }

    for (i = 0; i < 4; i++) {
        Pin_Write(PIN_OLED_SCLK, 0);
        Pin_Write(PIN_OLED_SDI, 0);
        Pin_Write(PIN_OLED_SCLK, 1);
    }

    // upper 4 bits
    for (i = 0; i < 4; i++) {
        Pin_Write(PIN_OLED_SCLK, 0);
        Pin_Write(PIN_OLED_SDI, c & 0x01);
        c = c >> 1;
        Pin_Write(PIN_OLED_SCLK, 1);
    }

    for (i = 0; i < 4; i++) {
        Pin_Write(PIN_OLED_SCLK, 0);
        Pin_Write(PIN_OLED_SDI, 0);
        Pin_Write(PIN_OLED_SCLK, 1);
    }
}

void OLED_command(unsigned char c) {
    _OLED_send_serial(c, 0xF8);
}

void OLED_data(unsigned char c) {
    _OLED_send_serial(c, 0xFA);
}

uint32_t _compute_line_hash(NHD_US2066_OLED *oled, int line) {
    if (line < 0 || line > OLED_NLINES) return 0;

    // according to some dude
    // https://stackoverflow.com/a/7666577
    uint32_t hash = 5381;
    for (int i = 0; i < OLED_NCOLS; i++) {
        int chr = (int) oled->buf[__pos2idx(oled, line, i)];
        hash = ((hash<<5) + hash) + chr;
    }

    return hash;
}

void oled_init(NHD_US2066_OLED *oled) {
    oled->nlines = OLED_NLINES;
    oled->ncols = OLED_NCOLS;

    oled->line = 0;
    oled->col = 0;

    oled->contrast = 0xFF; // max contrast

    int i;
    for (i = 0; i < oled->nlines; i++) {
        oled->lineupdates[i] = false;
    }

    // blank display
    for (i = 0; i < oled->nlines * oled->ncols; i++) {
        oled->buf[i] = ' ';
    }

    for (i = 0; i < oled->nlines; i++) {
        oled->line_hashes[i] = _compute_line_hash(oled, i);
    }
}

void oled_set_pos(NHD_US2066_OLED *oled, int line, int col) {
    if (col >= 0 && col < oled->ncols)
        oled->col = col;

    if (line >= 0 && line < oled->nlines)
        oled->line = line;
}

void _oled_print_generic(NHD_US2066_OLED *oled, char *str, bool wrap) {
    int i = 0;
    for (; str[i] != '\0'; i++) {
        // handle wrapping
        if (oled->col >= oled->ncols) {
            if (wrap) {
                if (oled->line + 1 < oled->nlines) {
                    oled->line += 1;
                    oled->col = 0;
                } else {
                    return;
                }
            } else {
                return;
            }
        }

        int idx = __pos2idx(oled, oled->line, oled->col);
        unsigned char c = str[i];

        if (oled->buf[idx] != c) {
            oled->buf[idx] = c;
            oled->lineupdates[oled->line] = true;
        }

        oled->col += 1;
    }
}

void oled_print_wrap(NHD_US2066_OLED *oled, char *str) {
    _oled_print_generic(oled, str, true);
}

void oled_print(NHD_US2066_OLED *oled, char *str) {
    _oled_print_generic(oled, str, false);
}

void oled_print_num(NHD_US2066_OLED *oled, int num) {
    char str[30] = {'\0'};
    itoa(num, str, 10);

    oled_print(oled, str);
}

void oled_print_char(NHD_US2066_OLED *oled, char chr) {
    // just create a one char string
    char str[2] = {chr, '\0'};
    oled_print(oled, str);
}

void _right_justified_pos(NHD_US2066_OLED *oled, int len) {
    int col = oled->ncols - len;
    if (col < 0)
        col = 0;

    oled->col = col;
}

void oled_rprint(NHD_US2066_OLED *oled, char *str) {
    oled_rprint_pad(oled, str, 0);
}

void oled_rprint_pad(NHD_US2066_OLED *oled, char *str, int pad) {
    int len = strlen(str);
    _right_justified_pos(oled, len + pad);
    oled_print(oled, str);
}

void oled_rprint_num(NHD_US2066_OLED *oled, int num) {
    oled_rprint_num_pad(oled, num, 0);
}

// print `num` divided by `div` to `decimals` decimal points
void oled_print_num_dec(NHD_US2066_OLED *oled, int num, int div, int decimals) {
    if (div <= 0) return;  // TODO: Find a way to notify of attempt to zerodiv.
    if (div % 10 != 0) return;  // TODO: Notifiy that div was not base 10.
    oled_print_num(oled, num / div);
    if (div > 1) {
        oled_print(oled, ".");
        for (int i = 0; i < decimals && div > 0; i++) {
            div /= 10;
            int dec = (num / div) % 10;
            char c = dec + '0';
            oled_print_char(oled, c);
        }
    }
}

void oled_rprint_num_pad(NHD_US2066_OLED *oled, int num, int pad) {
    char str[30] = {'\0'};
    itoa(num, str, 10);

    int len = strlen(str);
    _right_justified_pos(oled, len + pad);
    oled_print(oled, str);
}

void _oled_writeline(NHD_US2066_OLED *oled, int line) {
    int i;
    for (i = 0; i < oled->ncols; i++) {
        int idx = __pos2idx(oled, line, i);
        unsigned char c = oled->buf[idx];
        OLED_data(c);
    }
}

void oled_clear(NHD_US2066_OLED *oled) {
    OLED_command(0x01);

    int i;
    for (i = 0; i < oled->nlines * oled->ncols; i++) {
        oled->buf[i] = ' ';
    }
}

void oled_clearline(NHD_US2066_OLED *oled, int line) {
    oled->lineupdates[line] = true;

    int i;
    for (i = 0; i < oled->ncols; i++) {
        int idx = __pos2idx(oled, line, i);
        oled->buf[idx] = ' ';
    }
}

void oled_update(NHD_US2066_OLED *oled) {
    if (oled->nlines == NHD_0420CW_NLINES) {
        int i;
        unsigned char cmdarray[NHD_0420CW_NLINES] = {0x02, 0xA0, 0xC0, 0xE0};
        for (i = 0; i < NHD_0420CW_NLINES; i++) {
            if (oled->lineupdates[i]) {
                uint32_t hash = _compute_line_hash(oled, i);
                if (hash != oled->line_hashes[i]) {
                    oled->line_hashes[i] = hash;
                    OLED_command(cmdarray[i]);
                    _oled_writeline(oled, i);
                    oled->lineupdates[i] = false;
                }
            }
        }
    }
}

void oled_init_commands(NHD_US2066_OLED *oled) {
    OLED_command(0x2A);  //function set (extended command set)
	OLED_command(0x71);  //function selection A, disable internal Vdd regualtor
	OLED_data(0x00);
	OLED_command(0x28);  //function set (fundamental command set)
	OLED_command(0x08);  //display off, cursor off, blink off
	OLED_command(0x2A);  //function set (extended command set)
	OLED_command(0x79);  //OLED command set enabled
	OLED_command(0xD5);  //set display clock divide ratio/oscillator frequency
	OLED_command(0x70);  //set display clock divide ratio/oscillator frequency
	OLED_command(0x78);  //OLED command set disabled
	OLED_command(0x09);  //extended function set (4-lines)
    OLED_command(0x06);  //COM SEG direction
	OLED_command(0x72);  //function selection B, disable internal Vdd regualtor
	OLED_data(0x00);     //ROM CGRAM selection
	OLED_command(0x2A);  //function set (extended command set)
	OLED_command(0x79);  //OLED command set enabled
	OLED_command(0xDA);  //set SEG pins hardware configuration
	OLED_command(0x10);  //set SEG pins ... NOTE: When using NHD-0216AW-XB3 or NHD_0216MW_XB3 change to (0x00)
	OLED_command(0xDC);  //function selection C
	OLED_command(0x00);  //function selection C
	OLED_command(0x81);  //set contrast control
	OLED_command(oled->contrast);  //set contrast control
	OLED_command(0xD9);  //set phase length
	OLED_command(0xF1);  //set phase length
	OLED_command(0xDB);  //set VCOMH deselect level
	OLED_command(0x40);  //set VCOMH deselect level
	OLED_command(0x78);  //OLED command set disabled
	OLED_command(0x28);  //function set (fundamental command set)
	OLED_command(0x01);  //clear display
	OLED_command(0x80);  //set DDRAM address to 0x00
	OLED_command(0x0C);  //display ON
}

void oled_set_double_height_mode(NHD_US2066_OLED *oled,
    NHD_US2066_double_height_mode mode) {

    UNUSED(oled);

    if (mode == NHD_US2066_DOUBLE_HEIGHT_NONE) {
        OLED_command(0x28);
        return;
    }

    uint8_t disp_shift = 1;
    uint8_t UD = mode;
    unsigned char cmd = disp_shift | (UD << 2) | (1 << 4);

    //cmd = 0b00011001;

	//OLED_command(0x28);  //function set (fundamental command set)
    OLED_command(0b00101110);
    OLED_command(cmd);
    OLED_command(0b00101100);
}

void oled_set_clk_div(NHD_US2066_OLED *oled, uint8_t div, uint8_t freq) {
    if (div > 0xF) div = 0xF;
    if (freq > 0xF) freq = 0xF;

    unsigned char cmd = div | (freq << 4);
    OLED_command(0xD5);
    OLED_command(cmd);
}
