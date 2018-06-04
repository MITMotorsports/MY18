// FSAE 2018
// August Trollback
//
// Interface for OLEDs driven by an US2066 chip.

#ifndef _NHD_US2066_H_
#define _NHD_US2066_H_

#include <stdint.h>
#include <stdbool.h>

#include "NHD_US2066_charset.h"

#include "oled_config.h"

typedef struct {
    // need to be nlines * ncols long
    char buf[OLED_NLINES * OLED_NCOLS];
    bool lineupdates[OLED_NLINES];
    uint32_t line_hashes[OLED_NLINES];

    uint8_t contrast;

    int line, col;
    int nlines, ncols;
} NHD_US2066_OLED;

// Initialize an oled struct for a screen with nlines and ncols
void oled_init(NHD_US2066_OLED *oled);

// Run the commands needed to initialize the oled for viewing
void oled_init_commands(NHD_US2066_OLED *oled);

// Set the position to print next
void oled_set_pos(NHD_US2066_OLED *oled, int line, int col);

// Print a null terminated string at the current position with wrapping
void oled_print_wrap(NHD_US2066_OLED *oled, char *str);

// Print a null terminated string at the current position (no wrapping)
void oled_print(NHD_US2066_OLED *oled, char *str);

// Print a right justified string
void oled_rprint(NHD_US2066_OLED *oled, char *str);
// Print a right justified string with `pad` padding to the right
void oled_rprint_pad(NHD_US2066_OLED *oled, char *str, int pad);

// Print an integer number at the current position
void oled_print_num(NHD_US2066_OLED *oled, int num);
// Print a right justified number
void oled_rprint_num(NHD_US2066_OLED *oled, int num);
// Print a right justified number with `pad` padding to the right
void oled_rprint_num_pad(NHD_US2066_OLED *oled, int num, int pad);
//
// print `num` divided by `div` to `decimals` decimal points
void oled_print_num_dec(NHD_US2066_OLED *oled, int num, int div, int decimals);

// Print a single character at the current position.
// Useful for printing special symbols defined in NHD_US2066_charset.h
void oled_print_char(NHD_US2066_OLED *oled, char chr);

// Clear the oled screen
void oled_clear(NHD_US2066_OLED *oled);

// Clear a single oled line
void oled_clearline(NHD_US2066_OLED *oled, int line);

// Display updates to the screen
void oled_update(NHD_US2066_OLED *oled);

typedef enum {
    // lines: normal, normal, double
    NHD_US2066_DOUBLE_HEIGHT_BOTTOM = 0b00,

    // lines: normal, double, normal
    NHD_US2066_DOUBLE_HEIGHT_MIDDLE = 0b01,

    // lines: double, normal, normal
    NHD_US2066_DOUBLE_HEIGHT_TOP = 0b11,
    
    // lines: double, double
    NHD_US2066_DOUBLE_HEIGHT_BOTH = 0b10,

    NHD_US2066_DOUBLE_HEIGHT_NONE
} NHD_US2066_double_height_mode;

void oled_set_double_height_mode(NHD_US2066_OLED *oled,
    NHD_US2066_double_height_mode mode);

// Set clock division and oscillator frequency
//  - `div` must be between 0x0 and 0xF. Divides display clock by `div` + 1
//  - `freq` must be between 0x0 and 0xF. Sets oscillator frequency.
//     Larger value = higher frequency.
void oled_set_clk_div(NHD_US2066_OLED *oled, uint8_t div, uint8_t freq);

#endif
