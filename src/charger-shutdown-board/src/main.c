#include "main.h"

volatile uint32_t msTicks;
const uint32_t OscRateIn = 24000000;


can0_ChargerStatus1_T status;


static I2C_XFER_T xfer;
static uint8_t i2c_rx_buf[20];
static uint8_t i2c_tx_buf[20];

inline void delay (uint32_t time){
	uint32_t wait=msTicks;
	while(msTicks-wait <time){}
}

//START MCP23017 DRIVER
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


void init_MCP2307(void){
	send_i2c(0, MCP23017_IOCON);
	send_i2c_2(0,0, MCP23017_GPIO); 
	send_i2c_2(0x1F,0,MCP23017_IODIR); //lower 5 bits button input
	send_i2c(0x1F, MCP23017_GPPU); //pullup lower 5 bits
	send_i2c_2(0,0,MCP23017_GPINTEN);
	read_i2c(MCP23017_PORTA );
	read_i2c(MCP23017_PORTB );
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


//END MCP23017 DRIVER

//START DISPLAY DRIVER
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


const uint8_t _rs_pin=15;
const uint8_t _rw_pin=14;
const uint8_t _enable_pin=13;
const uint8_t _data_pins[4]={12,11,10,9};
const uint8_t _button_pins[5]={0,1,2,3,4};

const uint8_t displayfn=  LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
const uint8_t displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
const uint8_t displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;



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
	uint32_t wait;

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

}

void send(uint8_t value, uint8_t mode) {
  digital_write(_rs_pin, mode);
  // if there is a RW pin indicated, set it low to Write
    digital_write(_rw_pin, 0);
    write4bits(value>>4);
    write4bits(value);
}

	
//END DISPLAY DRIVER
void SysTick_Handler(void) {
  msTicks++;
}

void I2C_IRQHandler(void)
{
	if (Chip_I2C_IsMasterActive(I2C0)) {
		Chip_I2C_MasterStateHandler(I2C0);
	}
	else {
		Chip_I2C_SlaveStateHandler(I2C0);
	}
}

int main(void) {
  	SystemCoreClockUpdate();
  	Board_GPIO_Init();
	Board_UART_Init(57600);
  	Board_Print("H e l l o\n");
  	CAN_INIT();
  	Board_Print("CAN Initialized\n");

	if(SysTick_Config(SystemCoreClock/1000))
		while(1);
	//DON'T FORGET THIS A N G E R 	
		
//	// LPC_SYSCTL->SYSAHBCLKCTRL |= 0x20; 	// Enable clock and power to I2C block

	Chip_SYSCTL_DeassertPeriphReset(RESET_I2C0);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	xfer.txBuff = i2c_tx_buf;
	xfer.rxBuff = i2c_rx_buf;
	xfer.slaveAddr = MCP23017_ADDRESS;
	bool b=1;
	Board_Print("I2C Initialized\n");
	uint32_t wait=msTicks;
	//no more init
	init_MCP2307();
//	MCP2307_writeGPIOAB(0xFFFF);
	send_i2c(0, MCP23017_GPIO +1);	
	init_lcd();
	
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
       	while(1){
		advance_csb_state();
		can_receive();

			
	}


 	
	return 0;
}


void can_receive(void){
	Can_RawRead(&can_input);
	can0_T msgForm = identify_can0(&can_input);
	switch(msgForm){
	case can0_ChargerStatus1:
		can_receive_status_1();
		break;
	default:
		break;
	}
}

void can_receive_status_1(void){
	unpack_can0_ChargerStatus1(&can_input, &status);
}
