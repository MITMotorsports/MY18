#include "lcd.h"
static I2C_XFER_T xfer;
static uint8_t i2c_rx_buf[20];
static uint8_t i2c_tx_buf[20];


void init_MCP2307(void){
	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	Board_Print("I2C Initialized\n");

	send_i2c(0, MCP23017_IOCON);
	send_i2c_2(0,0, MCP23017_GPIO); 
	send_i2c_2(0x1F,0,MCP23017_IODIR); //lower 5 bits button input
	send_i2c(0x1F, MCP23017_GPPU); //pullup lower 5 bits
	send_i2c_2(0,0,MCP23017_GPINTEN);
	read_i2c(MCP23017_PORTA );
	read_i2c(MCP23017_PORTB );
	send_i2c(0, MCP23017_GPIO +1);
}

//To read GPIO call read_i2c(MCP23017_GPIO) for port a and read_i2c(MCP23017_GPIO+1) for port B
//

void read_i2c(uint8_t slave_register){
	Chip_I2C_MasterCmdRead(I2C0, xfer.slaveAddr, slave_register, i2c_rx_buf, 1);
}

void send_i2c_2(uint8_t slave_data1, uint8_t slave_data2, uint8_t slave_register){
	i2c_tx_buf[0] = slave_register;
	i2c_tx_buf[1] = slave_data1;
	i2c_tx_buf[2] = slave_data2;

	xfer.txSz = 3;

	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}

void send_i2c(uint8_t slave_data, uint8_t slave_register){
	i2c_tx_buf[0] = slave_register;
	i2c_tx_buf[1] = slave_data;
	xfer.txSz = 2;

	Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}


void digital_write(uint8_t pin, uint8_t data){
	uint8_t reg = MCP23017_GPIO;
	uint8_t output;
	if(pin>8){ //want to write to port b
		pin-=8;
		reg+=1; //write to port b
	}
	read_i2c(reg);//obtain current state of pins
	output=i2c_rx_buf[0];
	if(data!=0)
		output|=1<<pin;
	else
		output&=~(1<<pin);
	send_i2c(output,reg);

}

void pulseEnable(void) {
  digital_write(_enable_pin, 0);
  digital_write(_enable_pin, 1);
  _digital_write(_enable_pin, 0);
}

void write4bits(uint8_t data){
	uint8_t out;
	uint8_t reg = MCP23017_GPIO+1;

	read_i2c(reg);
	out=i2c_rx_buf[0]; //read port b

	for (int i = 0; i < 4; i++) {
	      out &= ~(1 << (_data_pins[i]-8));
	      out |= ((data >> i) & 0x1) << (_data_pins[i]-8);
    	}
	
	out &= ~(1 << (_enable_pin-8));
	send_i2c(out,reg); //Make sure to Ground enable

	//pulse
	out |= (1 << (_enable_pin-8));
	send_i2c(out,reg);
	out &= ~(1 << (_enable_pin-8));
	send_i2c(out,reg);

}

void command(uint8_t value){
	send(value,0);
}

void write(uint8_t value){
	send(value, 0xFF);
}

void write_str(char* str, int len){
	for(int i = 0; i<len ;i++){
		write(str[i]);
	}
}

void clear(){
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delay(3);
}

void display(){
  command(LCD_DISPLAYCONTROL  | displaycontrol);
}

void init_lcd(){
	
	uint32_t lcd_wait=msTicks;
	while(msTicks-lcd_wait<50)
	digital_write(_rs_pin,0);
	digital_write(_enable_pin,0);
	digital_write(_rw_pin,0);

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



	command(LCD_FUNCTIONSET |displayfn);  
  // turn the display on with no cursor or blinking default
//  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  //_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
	command(LCD_ENTRYMODESET|displaymode );
	command(0x32);
	command(0x32);//turn off white bars
	write_str("voltage:",8);
	command(LCD_SETDDRAMADDR | 0x40 ); //move the cursor to 2nd row
	write_str("current:",8);
//Update voltage and current values
	command(LCD_SETDDRAMADDR |  9);
	write_str("270", 3);
	command(LCD_SETDDRAMADDR | 9 + 0x40);
	write_str("008", 3);

}

void send(uint8_t value, uint8_t mode) {
  digital_write(_rs_pin, mode);
  // if there is a RW pin indicated, set it low to Write
    digital_write(_rw_pin, 0);
    write4bits(value>>4);
    write4bits(value);
}

inline void delay (uint32_t time){
	uint32_t wait=msTicks;
	while(msTicks-wait <time){}
}

