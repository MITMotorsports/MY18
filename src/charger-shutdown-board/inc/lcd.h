#ifndef _LCD_H_
#define _LCD_H_
#include "chip.h"
#include <stdint.h>

#define SCL 0, 4
#define SDA 0, 5
#define MCP23017_ADDRESS 0x20
#define MCP23017_IODIR 0x00
#define MCP23017_GPIO 0x12
#define MCP23017_IOCON 0x0A
#define MCP23017_GPINTEN 0x04
#define MCP23017_PORTA 0x00
#define MCP23017_PORTB 0x01
#define MCP23017_GPPU 0x0C


// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define BUTTON_UP 0x08
#define BUTTON_DOWN 0x04
#define BUTTON_LEFT 0x10
#define BUTTON_RIGHT 0x02
#define BUTTON_SELECT 0x01


extern volatile uint32_t msTicks;


 const uint8_t _rs_pin=15;

/*
const uint8_t _rw_pin=14;
const uint8_t _enable_pin=13;
const uint8_t _data_pins[4]={12,11,10,9};
const uint8_t _button_pins[5]={0,1,2,3,4};


const uint8_t displayfn=  LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
const uint8_t displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
const uint8_t displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
*/


void init_MCP2307(void);
void read_i2c(uint8_t);
void send_i2c_2(uint8_t slave_data1, uint8_t slve_data2, uint8_t slave_register);
void send_i2c(uint8_t slave_data, uint8_t slave_register);
void digital_write(uint8_t pin, uint8_t data);
void pulseEnable(void);
void write4bits(uint8_t data);
void command(uint8_t value);
void write(uint8_t value);
void write_str(char* str, int len);
void clear(void);
void display(void);

#endif //ifndef lcd.h
