#include "main.h"

#define BTN_STRINGIFY(val) ((val) ? "pressed\n" : "released\n")
#define OUT_STRINGIFY(val) ((val) ? "HIGH\n" : "LOW\n")
#define SET_STRUCT_ZERO(name) memset(&name, 0, sizeof(name));

volatile uint32_t msTicks;

int main(void) {
  SystemCoreClockUpdate();

  Board_UART_Init(UART_BAUD);

  Board_Print("CAN INITIALIZATION\n");
  CAN_Init();

  if (SysTick_Config(SystemCoreClock / 1000)) {
    while (1);  // error
  }

  Board_Print("DONE INITIALIZING\n");

  Board_Println("Currently running: "HASH);
  Board_Println("Flashed by: "AUTHOR);

  button_states_t hold;
  SET_STRUCT_ZERO(hold);
  Frame p;

  uint32_t last_buzz = 0;

  while (1) {
    // Clear RX buffer
    Can_RawRead(&p);

    button_states_t current = poll_buttons();
    hold.rtd          |= current.rtd;
    hold.driver_reset |= current.driver_reset;

    if (send_buttonrequest(hold)) {
      SET_STRUCT_ZERO(hold);
    }

    if (false && msTicks - last_buzz > 1000) {
      static bool buzz = false;
      SET_PIN(BUZZER, buzz);
      Board_Print("[BUZZER] ");
      Board_Println(OUT_STRINGIFY(buzz));

      buzz = !buzz;
      last_buzz = msTicks;
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
    Board_Print("RTD is ");
    Board_Println(BTN_STRINGIFY(bs.rtd));
  }

  if (bs.driver_reset != last_bs.driver_reset) {
    Board_Print("DriverReset is ");
    Board_Println(BTN_STRINGIFY(bs.driver_reset));
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
      Board_Print("Can_Error_NONE\n");
      break;

    case Can_Error_NO_RX:
      Board_Print("Can_Error_NO_RX\n");
      break;

    case Can_Error_EPASS:
      Board_Print("Can_Error_EPASS\n");
      break;

    case Can_Error_WARN:
      Board_Print("Can_Error_WARN\n");
      break;

    case Can_Error_BOFF:
      Board_Print("Can_Error_BOFF\n");
      break;

    case Can_Error_STUF:
      Board_Print("Can_Error_STUF\n");
      break;

    case Can_Error_FORM:
      Board_Print("Can_Error_FORM\n");
      break;

    case Can_Error_ACK:
      Board_Print("Can_Error_ACK\n");
      break;

    case Can_Error_BIT1:
      Board_Print("Can_Error_BIT1\n");
      break;

    case Can_Error_BIT0:
      Board_Print("Can_Error_BIT0\n");
      break;

    case Can_Error_CRC:
      Board_Print("Can_Error_CRC\n");
      break;

    case Can_Error_UNUSED:
      Board_Print("Can_Error_UNUSED\n");
      break;

    case Can_Error_UNRECOGNIZED_MSGOBJ:
      Board_Print("Can_Error_UNRECOGNIZED_MSGOBJ\n");
      break;

    case Can_Error_UNRECOGNIZED_ERROR:
      Board_Print("Can_Error_UNRECOGNIZED_ERROR\n");
      break;

    case Can_Error_TX_BUFFER_FULL:
      Board_Print("Can_Error_TX_BUFFER_FULL\n");
      CAN_Init();
      break;

    case Can_Error_RX_BUFFER_FULL:
      Board_Print("Can_Error_RX_BUFFER_FULL\n");
      CAN_Init();
      break;
    }
  }
}

// THE NETHER REGIONS -- TREAD CAREFULLY
const uint32_t OscRateIn = 12000000;

void SysTick_Handler(void) {
  msTicks++;
}

void GPIO_Init(void) {
  /// INPUTS
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, DRIVER_RST);
  Chip_IOCON_PinMuxSet(LPC_IOCON, DRIVER_RST_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, SCROLL_SEL);
  Chip_IOCON_PinMuxSet(LPC_IOCON, SCROLL_SEL_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, RTD);
  Chip_IOCON_PinMuxSet(LPC_IOCON, RTD_IOCON, BTN_CONFIG);

  /// OUTPUTS
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, RTD);
  Chip_IOCON_PinMuxSet(LPC_IOCON, RTD_IOCON, BUZZER_CONFIG);

  Chip_GPIO_Init(LPC_GPIO);
}
