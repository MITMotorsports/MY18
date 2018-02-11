#ifndef _NHD_US2066_H_
#define _NHD_US2066_H_

#define NHD_0420CW_NLINES 4
#define NHD_0420CW_NCOLS  20

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // need to be nlines * ncols long
    char buf[NHD_0420CW_NLINES * NHD_0420CW_NCOLS];
    bool lineupdates[NHD_0420CW_NLINES];

    uint8_t contrast;

    int line, col;
    int nlines, ncols;
} NHD_US2066_OLED;

void oled_init(NHD_US2066_OLED *oled, int nlines, int ncols);

void oled_init_commands(NHD_US2066_OLED *oled);

void oled_set_pos(NHD_US2066_OLED *oled, int line, int col);

// print a null terminated string
void oled_print_wrap(NHD_US2066_OLED *oled, char *str);
void oled_print(NHD_US2066_OLED *oled, char *str);

void oled_clear(NHD_US2066_OLED *oled);
void oled_update(NHD_US2066_OLED *oled);

#endif
