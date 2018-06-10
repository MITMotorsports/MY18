#define _BUTTONBANK_V2_
#include "main.h"

#define DEBUG_MODE true

#define OUT_STRINGIFY(val) ((val) ? "HIGH\n" : "LOW\n")
#define VCU_STATE(name) can0_VCUHeartbeat_vcu_state_VCU_STATE_ ## name

bool button_state[LEN_BUTTONS] = {};
car_states_t car_state = {};

volatile uint32_t msTicks;

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

  SET_PIN(DRIVER_RST_LED, LED_OFF);
  SET_PIN(RTD_LED, LED_OFF);
  SET_PIN(BTN_A_LED, LED_OFF);
  SET_PIN(BTN_B_LED, LED_OFF);
  SET_PIN(BUZZER, false);  // OFF

  while (1) {
    // Button handling strategy
    //   1. Poll all buttons
    poll_buttons(button_state);

    //   2. Debounce all buttons
    static bool     debounce_state[LEN_BUTTONS] = {};
    static uint32_t debounce_edge[LEN_BUTTONS]  = {};

    static bool button_debounced[LEN_BUTTONS] = {};

    for (BUTTONS i = 0; i < LEN_BUTTONS; ++i) {
      if (button_state[i]) {
        if (!debounce_state[i]) {
          debounce_edge[i] = msTicks;
          debounce_state[i] = true;
        }
        else if (msTicks - debounce_edge[i] > DEBOUNCE_SETUP) {
          button_debounced[i] = true;
        }
      }
      else {
        if (debounce_state[i]) {
          debounce_edge[i] = msTicks;
          debounce_state[i] = false;
        }
        else if (msTicks - debounce_edge[i] > DEBOUNCE_HOLD) {
          button_debounced[i] = false;
        }
      }
    }

    //   3. Send it bruh.
    send_buttonrequest(button_debounced);

    // All of the rest functionality
    can_read();
    buzz();
    button_leds();
    #if DEBUG_MODE
      print_buttons(button_debounced);
    #endif
  }

  return 0;
}

void poll_buttons(bool* button_state) {
  button_state[rtd]           = (READ_PIN(BTN_RTD)        == BTN_DOWN);
  button_state[driver_reset]  = (READ_PIN(BTN_DRIVER_RST) == BTN_DOWN);
  button_state[A]             = (READ_PIN(BTN_A) == BTN_DOWN);
  button_state[B]             = (READ_PIN(BTN_B) == BTN_DOWN);
}

can0_VCUHeartbeat_vcu_state_T get_vcu_state(void) {
  static can0_VCUHeartbeat_vcu_state_T last_state = VCU_STATE(ROOT);
  static can0_VCUHeartbeat_vcu_state_T used_state = VCU_STATE(ROOT);
  static uint32_t edge_time = 0;

  if (car_state.vcu_state != last_state) {
    edge_time = msTicks;
    last_state = car_state.vcu_state;
  }

  if (car_state.vcu_state != used_state) {
    if (msTicks - edge_time > VCU_INTRO_TIME) {
      used_state = car_state.vcu_state;
    }
  }

  return used_state;
}

void buzz(void) {
  static bool last = false;
  static uint32_t last_start = 0;
  bool current = (get_vcu_state() == VCU_STATE(DRIVING));

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


// TODO: Figure out why August named the buttons backwards.
void button_leds(void) {
  static uint32_t next_led_blink = 0;

  switch (get_vcu_state()) {
  case VCU_STATE(ROOT):
    SET_PIN(RTD_LED, LED_ON);
    SET_PIN(DRIVER_RST_LED, LED_ON);
    break;

  case VCU_STATE(LV):
    SET_PIN(DRIVER_RST_LED, LED_OFF);
    if (msTicks > next_led_blink) {
      SET_PIN(RTD_LED, !READ_PIN(RTD_LED)); // toggle
      next_led_blink = msTicks + LED_BLINK_PERIOD;
    }
    break;

  case VCU_STATE(PRECHARGING):
    SET_PIN(DRIVER_RST_LED, LED_OFF);
    SET_PIN(RTD_LED, LED_ON);
    break;

  case VCU_STATE(RTD):
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

void print_buttons(bool* bs) {
  static bool last_bs[LEN_BUTTONS] = {};

  #define BTN_STRINGIFY(val) ((val) ? "pressed\n" : "released\n")
  #define EZ_PRINT(name) if (bs[name] != last_bs[name]) { \
    Board_Print(#name" is ");                             \
    Board_Println(BTN_STRINGIFY(bs[name]));               \
  }

  // Print the button states on condition that they are different than before
  EZ_PRINT(rtd);
  EZ_PRINT(driver_reset);
  EZ_PRINT(A);
  EZ_PRINT(B);

  // Update the current button state
  memcpy(last_bs, bs, sizeof(last_bs));

  #undef BTN_STRINGIFY
  #undef EZ_PRINT
}

bool send_buttonrequest(bool* bs) {
  static uint32_t last_time = 0;

  if (msTicks - last_time > can0_ButtonRequest_period) {
    last_time = msTicks;

    can0_ButtonRequest_T msg;

    msg.RTD          = bs[rtd];
    msg.DriverReset  = bs[driver_reset];
    msg.A            = bs[A];
    msg.B            = bs[B];

    can_error_handler(can0_ButtonRequest_Write(&msg));

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
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, BTN_DRIVER_RST);
  Chip_IOCON_PinMuxSet(LPC_IOCON, BTN_DRIVER_RST_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, BTN_RTD);
  Chip_IOCON_PinMuxSet(LPC_IOCON, BTN_RTD_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, BTN_A);
  Chip_IOCON_PinMuxSet(LPC_IOCON, BTN_A_IOCON, BTN_CONFIG);

  Chip_GPIO_SetPinDIRInput(LPC_GPIO, BTN_B);
  Chip_IOCON_PinMuxSet(LPC_IOCON, BTN_B_IOCON, BTN_CONFIG);

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
