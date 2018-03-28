#include "main.h"

#define BTN_STRINGIFY(val) ((val) ? "pressed\n" : "released\n")
#define SET_STRUCT_ZERO(name) memset(&name, 0, sizeof(name));

volatile uint32_t msTicks;

int main(void) {
  SystemCoreClockUpdate();

  Serial_Init(UART_BAUD);

  print("CAN INITIALIZATION\n");
  CAN_Init();

  if (SysTick_Config(SystemCoreClock / 1000)) {
    while (1);  // error
  }

  print("DONE INITIALIZING\n");

  button_states_t hold;
  SET_STRUCT_ZERO(hold);
  Frame p;

  while (1) {
    // Clear RX buffer
    Can_RawRead(&p);

    button_states_t current = poll_buttons();
    hold.rtd          |= current.rtd;
    hold.driver_reset |= current.driver_reset;

    if (send_buttonrequest(hold)) {
      SET_STRUCT_ZERO(hold);
    }

    print_buttons(current);
  }

  return 0;
}

button_states_t poll_buttons(void) {
  button_states_t bs;

  bs.rtd          = (READ_PIN(RTD) == BTN_DOWN);
  bs.driver_reset = (READ_PIN(DRIVER_RST) == BTN_DOWN);
  return bs;
}

void print_buttons(button_states_t bs) {
  static button_states_t last_bs = {};

  if (bs.rtd != last_bs.rtd) {
    print("RTD is ");
    print(BTN_STRINGIFY(bs.rtd));
    print("\n\n");
  }

  if (bs.driver_reset != last_bs.driver_reset) {
    print("DriverReset is ");
    print(BTN_STRINGIFY(bs.driver_reset));
    print("\n\n");
  }

  last_bs = bs;
}

bool send_buttonrequest(button_states_t hold) {
  static uint32_t timer;

  if (msTicks > timer) {
    timer = msTicks + can0_ButtonRequest_period;

    // can0_ButtonRequest_T msg;
    // msg.RTD         = hold.rtd;
    // msg.DriverReset = hold.driver_reset;
    //
    // hold.rtd          = false;
    // hold.driver_reset = false;
    //
    // handle_can_error(can0_ButtonRequest_Write(&msg));

    Frame manual;

    manual.id       = can0_ButtonRequest_can_id;
    manual.len      = 1;
    manual.data[0]  = 0;
    manual.data[0] += (hold.rtd) ? 2 : 0;
    manual.data[0] += (hold.driver_reset) ? 4 : 0;

    handle_can_error(Can_RawWrite(&manual));

    hold.rtd = hold.driver_reset = 0;

    return true;
  }

  return false;
}

void handle_can_error(Can_ErrorID_T error) {
  if ((error != Can_Error_NONE) && (error != Can_Error_NO_RX)) {
    switch (error) {
    case Can_Error_NONE:
      print("Can_Error_NONE\n");
      break;

    case Can_Error_NO_RX:
      print("Can_Error_NO_RX\n");
      break;

    case Can_Error_EPASS:
      print("Can_Error_EPASS\n");
      break;

    case Can_Error_WARN:
      print("Can_Error_WARN\n");
      break;

    case Can_Error_BOFF:
      print("Can_Error_BOFF\n");
      break;

    case Can_Error_STUF:
      print("Can_Error_STUF\n");
      break;

    case Can_Error_FORM:
      print("Can_Error_FORM\n");
      break;

    case Can_Error_ACK:
      print("Can_Error_ACK\n");
      break;

    case Can_Error_BIT1:
      print("Can_Error_BIT1\n");
      break;

    case Can_Error_BIT0:
      print("Can_Error_BIT0\n");
      break;

    case Can_Error_CRC:
      print("Can_Error_CRC\n");
      break;

    case Can_Error_UNUSED:
      print("Can_Error_UNUSED\n");
      break;

    case Can_Error_UNRECOGNIZED_MSGOBJ:
      print("Can_Error_UNRECOGNIZED_MSGOBJ\n");
      break;

    case Can_Error_UNRECOGNIZED_ERROR:
      print("Can_Error_UNRECOGNIZED_ERROR\n");
      break;

    case Can_Error_TX_BUFFER_FULL:
      print("Can_Error_TX_BUFFER_FULL\n");
      CAN_Init();
      break;

    case Can_Error_RX_BUFFER_FULL:
      print("Can_Error_RX_BUFFER_FULL\n");
      CAN_Init();
      break;
    }
  }
}

// THE NETHER REGIONS -- TREAD CAREFULLY
const uint32_t    OscRateIn = 12000000;

void SysTick_Handler(void) {
  msTicks++;
}

void Serial_Init(uint32_t baudrate) {
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));

  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, baudrate);

  // Configure data width, parity, and stop bits
  Chip_UART_ConfigData(LPC_USART,
                       (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT |
                        UART_LCR_PARITY_DIS));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);
}

void print(const char *str) {
  Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}

void GPIO_Init(void) {
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, DRIVER_RST);
  Chip_IOCON_PinMuxSet(LPC_IOCON, DRIVER_RST_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, SCROLL_SEL);
  Chip_IOCON_PinMuxSet(LPC_IOCON, SCROLL_SEL_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, RTD);
  Chip_IOCON_PinMuxSet(LPC_IOCON, RTD_IOCON, BTN_CONFIG);

  Chip_GPIO_Init(LPC_GPIO);
}
