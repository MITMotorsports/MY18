#define _BUTTONBANK_V2_

#include "main.h"

#define BTN_STRINGIFY(val) ((val) ? "pressed\n" : "released\n")
#define OUT_STRINGIFY(val) ((val) ? "HIGH\n" : "LOW\n")
#define SET_STRUCT_ZERO(name) memset(&name, 0, sizeof(name));

car_states_t car_state = {};
volatile uint32_t msTicks;

void buzz(void);
void button_leds(void);
void can_read(void);

uint32_t next_led_blink = 0;
#define LED_BLINK_PERIOD 500

int main(void) {
  SystemCoreClockUpdate();
  GPIO_Init();

  Board_UART_Init(UART_BAUD);

  Board_Print("CAN INITIALIZATION\n");
  CAN_Init();

  Board_Println("Currently running: "HASH);
  Board_Println("Flashed by: "AUTHOR);

  if (SysTick_Config(SystemCoreClock / 1000)) {
    while (1);  // error
  }

  Board_Print("DONE INITIALIZING\n");

  next_led_blink = 0;

  SET_PIN(DRIVER_RST_LED, LED_OFF);
  SET_PIN(RTD_LED, LED_OFF);
  SET_PIN(BTN_A_LED, LED_OFF);
  SET_PIN(BTN_B_LED, LED_OFF);

  button_states_t hold;
  SET_STRUCT_ZERO(hold);

  uint32_t last_buzz = 0;

  SET_PIN(BUZZER, false);

  while (1) {
    // Update from CAN bus
    can_read();

    button_states_t current = poll_buttons();
    hold.rtd           |= current.rtd;
    hold.driver_reset  |= current.driver_reset;
    hold.scroll_select |= current.scroll_select;

    if (send_buttonrequest(hold)) {
      SET_STRUCT_ZERO(hold);
    }

    buzz();
    button_leds();

    print_buttons(current);
  }

  return 0;
}

button_states_t poll_buttons(void) {
  button_states_t bs;

  bs.rtd           = (READ_PIN(RTD)        == BTN_DOWN);
  bs.driver_reset  = (READ_PIN(DRIVER_RST) == BTN_DOWN);
  bs.scroll_select = (READ_PIN(SCROLL_SEL) == BTN_DOWN);

  return bs;
}

void buzz(void) {
  static bool last = false;
  static uint32_t last_start = 0;
  bool current = (car_state.vcu_state == can0_VCUHeartbeat_vcu_state_VCU_STATE_RTD);

  bool buzz = false;
  if (current) {
    if (last) {
      if (msTicks - last_start < BUZZ_DURATION) {
        buzz = true;
      }
    }
    else {
      last_start = msTicks;
      buzz = true;
    }
  }

  last = current;

  SET_PIN(BUZZER, buzz);
}

void button_leds(void) {
    switch (car_state.vcu_state) {
        case can0_VCUHeartbeat_vcu_state_VCU_STATE_LV:
        SET_PIN(DRIVER_RST_LED, LED_OFF);
        if (msTicks > next_led_blink) {
            SET_PIN(RTD_LED, !READ_PIN(RTD_LED)); // toggle
            next_led_blink = msTicks + LED_BLINK_PERIOD;
        }
        break;

        case can0_VCUHeartbeat_vcu_state_VCU_STATE_RTD:
        SET_PIN(RTD_LED, LED_OFF);
        if (msTicks > next_led_blink) {
            SET_PIN(DRIVER_RST_LED, !READ_PIN(DRIVER_RST_LED)); // toggle
            next_led_blink = msTicks + LED_BLINK_PERIOD;
        }
        break;

        default:
        SET_PIN(DRIVER_RST_LED, LED_OFF);
        SET_PIN(RTD_LED, LED_OFF);
        break;
    }
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

  if (bs.scroll_select != last_bs.scroll_select) {
    Board_Print("ScrollSelect is ");
    Board_Println(BTN_STRINGIFY(bs.scroll_select));
  }

  // this breaks everything for some reason
  //last_bs = bs;
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
    // can_error_handler(can0_ButtonRequest_Write(&msg));

    Frame manual;

    manual.id       = can0_ButtonRequest_can_id;
    manual.len      = 1;
    manual.data[0]  = 0;
    manual.data[0] += (hold.rtd) ? 2 : 0;
    manual.data[0] += (hold.driver_reset) ? 4 : 0;
    manual.data[0] += (hold.scroll_select) ? 8 : 0;

    can_error_handler(Can_RawWrite(&manual));

    hold.rtd = 0;
    hold.driver_reset = 0;

    return true;
  }

  return false;
}

void handle_vcu_heartbeat(Frame *frame) {
  can0_VCUHeartbeat_T msg;
  unpack_can0_VCUHeartbeat(frame, &msg);

  car_state.vcu_state   = msg.vcu_state;
  car_state.error_state = msg.error_state;
}

void can_read(void) {
  Frame raw;
  Can_RawRead(&raw);

  switch (identify_can0(&raw)) {
    case can0_VCUHeartbeat:
      handle_vcu_heartbeat(&raw);
      break;
    default:
      break;
  }
}

void can_error_handler(Can_ErrorID_T error) {
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
  Chip_GPIO_Init(LPC_GPIO);

  /// INPUTS
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, DRIVER_RST);
  Chip_IOCON_PinMuxSet(LPC_IOCON, DRIVER_RST_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, SCROLL_SEL);
  Chip_IOCON_PinMuxSet(LPC_IOCON, SCROLL_SEL_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, RTD);
  Chip_IOCON_PinMuxSet(LPC_IOCON, RTD_IOCON, BTN_CONFIG);

  /// OUTPUTS
  // Chip_GPIO_SetPinDIROutput(LPC_GPIO, BUZZER);
  // Chip_IOCON_PinMuxSet(LPC_IOCON, BUZZER_IOCON, BUZZER_CONFIG);
  Chip_GPIO_WriteDirBit(LPC_GPIO, BUZZER, true);

  Chip_GPIO_WriteDirBit(LPC_GPIO, RTD_LED, true);
  Chip_GPIO_WriteDirBit(LPC_GPIO, DRIVER_RST_LED, true);
  Chip_GPIO_WriteDirBit(LPC_GPIO, BTN_A_LED, true);
  Chip_GPIO_WriteDirBit(LPC_GPIO, BTN_B_LED, true);

  Chip_IOCON_PinMuxSet(LPC_IOCON, RTD_LED_IOCON, IOCON_FUNC0);
  Chip_IOCON_PinMuxSet(LPC_IOCON, DRIVER_RST_LED_IOCON, IOCON_FUNC1);
  Chip_IOCON_PinMuxSet(LPC_IOCON, BTN_A_LED_IOCON, IOCON_FUNC0);
  Chip_IOCON_PinMuxSet(LPC_IOCON, BTN_B_LED_IOCON, IOCON_FUNC0);
}
