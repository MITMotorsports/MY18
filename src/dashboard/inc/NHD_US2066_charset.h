#ifndef _NHD_US2066_CHARSET_H_
#define _NHD_US2066_CHARSET_H_

// Parts of US2066 controller charset. Excludes many ASCII
// characters since a char literal will work fine. Many
// accented characters also not included.

#define CHAR_RIGHT_TRIANGLE     0x10
#define CHAR_LEFT_TRIANGLE      0x11
#define CHAR_TWO_TRIANGLES_UP   0x12
#define CHAR_TWO_TRIANGLES_DOWN 0x13
#define CHAR_TWO_CHEVRONS_LEFT  0x14
#define CHAR_TWO_CHEVRONS_RIGHT 0x15
#define CHAR_ARROW_UPPER_LEFT   0x16
#define CHAR_ARROW_UPPER_RIGHT  0x17
#define CHAR_ARROW_LOWER_LEFT   0x18
#define CHAR_ARROW_LOWER_RIGHT  0x19
#define CHAR_TRIANGLE_UP        0x1A
#define CHAR_TRIANGLE_DOWN      0x1B
#define CHAR_RETURN             0x1C
#define CHAR_CHEVRON_UP         0x1D
#define CHAR_CHEVRON_DOWN       0x1E
#define CHAR_FULL_BLOCK         0x1F

#define CHAR_BLANK              0x20
#define CHAR_BANG               0x21
#define CHAR_DOUBLE_QUOTE       0x22
#define CHAR_POUND              0x23
#define CHAR_SQUARE_WITH_DOTS   0x24
#define CHAR_PERCENT            0x25
#define CHAR_AMPERSAND          0x26
#define CHAR_APOSTROPHE         0x27
#define CHAR_LEFT_PAREN         0x28
#define CHAR_RIGHT_PAREN        0x29
#define CHAR_ASTERISK           0x2A
#define CHAR_PLUS               0x2B
#define CHAR_COMMA              0x2C
#define CHAR_MINUS              0x2D
#define CHAR_PERIOD             0x2E
#define CHAR_FORWARD_SLASH      0x2F

#define CHAR_EXPONENT_0         0x80
#define CHAR_EXPONENT_1         0x81
#define CHAR_EXPONENT_2         0x82
#define CHAR_EXPONENT_3         0x83
#define CHAR_EXPONENT_4         0x84
#define CHAR_EXPONENT_5         0x85
#define CHAR_EXPONENT_6         0x86
#define CHAR_EXPONENT_7         0x87
#define CHAR_EXPONENT_8         0x88
#define CHAR_EXPONENT_9         0x89
#define CHAR_FRACTION_HALF      0x8A
#define CHAR_FRACTION_QUARTER   0x8B
#define CHAR_PLUS_MINUS         0x8C
#define CHAR_GTE                0x8D // greater than or equal
#define CHAR_LTE                0x8E // less than or equal
#define CHAR_MU                 0x8F

#define CHAR_SINGLE_NOTE        0x90
#define CHAR_DOUBLE_NOTE        0x91
#define CHAR_BELL               0x92
#define CHAR_HEART              0x93
#define CHAR_DIAMOND            0x94
#define CHAR_DIVIDER1           0x95 // don't know what this is
#define CHAR_UPPER_LEFT_CORNER  0x96
#define CHAR_LOWER_RIGHT_CORNER 0x97
#define CHAR_LEFT_DOUBLE_QUOTE  0x98
#define CHAR_RIGHT_DOUBLE_QUOTE 0x99
#define CHAR_LEFT_PAREN2        0x9A
#define CHAR_RIGHT_PAREN2       0x9B
#define CHAR_SMALL_ALPHA        0x9C
#define CHAR_SMALL_EPSILON      0x9D
#define CHAR_SMALL_DELTA        0x9E
#define CHAR_INFINITY           0x9F

#define CHAR_AT_SIGN            0xA0
#define CHAR_POUND_SIGN         0xA1
#define CHAR_DOLLAR_SIGN        0xA2
#define CHAR_YEN_SIGN           0xA3

#define CHAR_CAPITAL_DELTA      0xB0
#define CHAR_CENT_SIGN          0xB1
#define CHAR_CAPITAL_PHI        0xB2
#define CHAR_SMALL_TAU          0xB3
#define CHAR_SMALL_LAMBDA       0xB4
#define CHAR_CAPITAL_OMEGA      0xB5
#define CHAR_PI                 0xB6
#define CHAR_PSI                0xB7
#define CHAR_CAPITAL_SIGMA      0xB8
#define CHAR_SMALL_THETA        0xB9
#define CHAR_CAPITAL_XI         0xBA
#define CHAR_CIRCLE             0xBB
#define CHAR_CAPITAL_AE         0xBC
#define CHAR_SMALL_AE           0xBD
#define CHAR_SMALL_BETA         0xBE

#define CHAR_CAPITAL_GAMMA      0xC0
#define CHAR_CAPITAL_LAMBDA     0xC1
#define CHAR_CAPITAL_PI         0xC2
// 0xC3
#define CHAR_UNDERSCORE         0xC4
// 0xC5 .. 0xCC
#define CHAR_SMALL_ONE          0xCD
#define CHAR_TILDE              0xCE
#define CHAR_DIAMOND_HOLLOW     0xCF

#define CHAR_BLOCK1_WIDTH5      0xD0
#define CHAR_BLOCK1_WIDTH4      0xD1
#define CHAR_BLOCK1_WIDTH3      0xD2
#define CHAR_BLOCK1_WIDTH2      0xD3
#define CHAR_BLOCK1_WIDTH1      0xD4
#define CHAR_SMALL_F_HOOK       0xD5 // 'fancy' f
#define CHAR_BLOCK2_WIDTH5      0xD6
#define CHAR_BLOCK2_WIDTH4      0xD7
#define CHAR_BLOCK2_WIDTH3      0xD8
#define CHAR_BLOCK2_WIDTH2      0xD9
#define CHAR_BLOCK2_WIDTH1      0xDA
#define CHAR_PT                 0xDB // don't know what this means
#define CHAR_SQUARE_HOLLOW      0xDC
#define CHAR_CENTER_DOT         0xDD
#define CHAR_ARROW_UP           0xDE
#define CHAR_ARROW_RIGHT        0xDF

#define CHAR_ARROW_DOWN         0xE0
#define CHAR_ARROW_LEFT         0xE1

#define CHAR_LEFT_BRACKET       0xFA
#define CHAR_BACKWARD_SLASH     0xFB
#define CHAR_RIGHT_BRACKET      0xFC
#define CHAR_LEFT_BRACE         0xFD
#define CHAR_PIPE               0xFE
#define CHAR_RIGHT_BRACE        0xFF


#endif
