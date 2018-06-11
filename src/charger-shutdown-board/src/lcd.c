#include "lcd.h"
static I2C_XFER_T xfer;
static uint8_t    i2c_rx_buf[20];
static uint8_t    i2c_tx_buf[20];

void I2C_IRQHandler(void) {
  if (Chip_I2C_IsMasterActive(I2C0)) {
    Chip_I2C_MasterStateHandler(I2C0);
  }
  else {
    Chip_I2C_SlaveStateHandler(I2C0);
  }
}

void init_MCP2307(void) {
  Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
  Chip_I2C_Init(I2C0);
  Chip_I2C_SetClockRate(I2C0, 100000);

  Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
  NVIC_EnableIRQ(I2C0_IRQn);

  Board_Print("I2C Initialized\n");

  xfer.txBuff    = i2c_tx_buf;
  xfer.rxBuff    = i2c_rx_buf;
  xfer.slaveAddr = MCP23017_ADDRESS;

  send_i2c(0, MCP23017_IOCON);
  send_i2c_2(0,    0, MCP23017_GPIO);
  send_i2c_2(0x1F, 0, MCP23017_IODIR); // lower 5 bits button input
  send_i2c(0x1F, MCP23017_GPPU);       // pullup lower 5 bits
  send_i2c_2(0, 0, MCP23017_GPINTEN);
  read_i2c(MCP23017_PORTA);
  read_i2c(MCP23017_PORTB);
  send_i2c(0, MCP23017_GPIO + 1);
  Board_Print("Done MCP Init\n");
}

// To read GPIO call read_i2c(MCP23017_GPIO) for port a and
// read_i2c(MCP23017_GPIO+1) for port B
//

void read_i2c(uint8_t slave_register) {
  Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, slave_register, i2c_rx_buf, 1);
}

void send_i2c_2(uint8_t slave_data1, uint8_t slave_data2,
                uint8_t slave_register) {
  i2c_tx_buf[0] = slave_register;
  i2c_tx_buf[1] = slave_data1;
  i2c_tx_buf[2] = slave_data2;

  xfer.txSz = 3;

  Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}

void send_i2c(uint8_t slave_data, uint8_t slave_register) {
  i2c_tx_buf[0] = slave_register;
  i2c_tx_buf[1] = slave_data;
  xfer.txSz     = 2;

  Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}

static void digital_write(uint8_t pin, uint8_t data) {
  uint8_t reg = MCP23017_GPIO;
  uint8_t output;

  if (pin > 8) { // want to write to port b
    pin -= 8;
    reg += 1;    // write to port b
  }
  read_i2c(reg); // obtain current state of pins
  output = i2c_rx_buf[0];

  if (data != 0) output |= 1 << pin;
  else output &= ~(1 << pin);
  send_i2c(output, reg);
}

static void pulseEnable(void) {
  digital_write(_enable_pin, 0);
  digital_write(_enable_pin, 1);
  _digital_write(_enable_pin, 0);
}

static void write4bits(uint8_t data) {
  uint8_t out;
  uint8_t reg = MCP23017_GPIO + 1;

  read_i2c(reg);
  out = i2c_rx_buf[0]; // read port b

  for (int i = 0; i < 4; i++) {
    out &= ~(1 << (_data_pins[i] - 8));
    out |= ((data >> i) & 0x1) << (_data_pins[i] - 8);
  }

  out &= ~(1 << (_enable_pin - 8));
  send_i2c(out, reg); // Make sure to Ground enable

  // pulse
  out |= (1 << (_enable_pin - 8));
  send_i2c(out, reg);
  out &= ~(1 << (_enable_pin - 8));
  send_i2c(out, reg);
}

static void lcd_command(uint8_t value) {
  lcd_send(value, 0);
}

void init_lcd(void) {
  uint32_t lcd_wait = msTicks;

  while (msTicks - lcd_wait < 50) digital_write(_rs_pin, 0);
  digital_write(_enable_pin, 0);
  digital_write(_rw_pin,     0);

  write4bits(0x03);
  delay(5);

  // second try
  write4bits(0x03);
  delay(5);

  // third go!
  write4bits(0x03);

  delay(1);

  // finally, set to 8-bit interface
  write4bits(0x02);


  lcd_command(LCD_FUNCTIONSET | displayfn);

  // turn the display on with no cursor or blinking default
  //  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  lcd_display();

  // clear it off
  lcd_clear();

  // Initialize to default text direction (for romance languages)
  // _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  lcd_command(LCD_ENTRYMODESET | displaymode);
  lcd_command(0x32);
  lcd_command(0x32); // turn off white bars
}

static void lcd_send(uint8_t value, uint8_t mode) {
  digital_write(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  digital_write(_rw_pin, 0);
  write4bits(value >> 4);
  write4bits(value);
}

inline void lcd_write(uint8_t value) {
  lcd_send(value, 0xFF);
}

void lcd_print(char *str) {
  while (*str != '\0') lcd_write(*(str++));
}

void lcd_print_num(int32_t num, unsigned base) {
  char buff[20] = {'\0'};

  itoa(num, buff, base);
  lcd_print(buff);
}

// print `num` divided by `div` to `decimals` decimal points
void lcd_print_num_dec(int num, int div, int decimals) {
  if (div <= 0) return;  // TODO: Find a way to notify of attempt to zerodiv.
  if (div % 10 != 0) return;  // TODO: Notifiy that div was not base 10.
  lcd_print_num(num / div, 10);
  if (div > 1) {
    lcd_write('.');
    for (int i = 0; i < decimals && div > 0; i++) {
      // TODO: Optimize with itoa and buff[decimals] = '\0'.
      div /= 10;
      int dec = (num / div) % 10;
      char c = dec + '0';
      lcd_write(c);
    }
  }
}

void lcd_clear() {
  lcd_command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
  // delay(3);
}

void lcd_display() {
  lcd_command(LCD_DISPLAYCONTROL | displaycontrol);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

  lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Set the cursor and clear `len` characters ahead.
void lcd_set_cursor_clearahead(uint8_t col, uint8_t row, uint8_t len) {
  lcd_set_cursor(col, row);
  while (len--) lcd_write(' ');
  lcd_set_cursor(col, row);
}

inline void delay(uint32_t time) {
  uint32_t wait = msTicks;

  while (msTicks - wait < time) {}
}
