#include "knob.h"
#include "board.h"

#define TOLERANCE 8

#define POS0_VAL 0
#define POS1_VAL 205
#define POS2_VAL 407
#define POS3_VAL 614
#define POS4_VAL 819
#define POS5_VAL 931

int get_knob_position(void) {
    uint16_t val = ADC_Read(KNOB_CHANNEL);

    if (val < POS0_VAL + TOLERANCE)
        return 0;
    if (val > POS1_VAL - TOLERANCE && val < POS1_VAL + TOLERANCE)
        return 1;
    if (val > POS2_VAL - TOLERANCE && val < POS2_VAL + TOLERANCE)
        return 2;
    if (val > POS3_VAL - TOLERANCE && val < POS3_VAL + TOLERANCE)
        return 3;
    if (val > POS4_VAL - TOLERANCE && val < POS4_VAL + TOLERANCE)
        return 4;
    if (val > POS5_VAL - TOLERANCE && val < POS5_VAL + TOLERANCE)
        return 5;

    return -1;
}
