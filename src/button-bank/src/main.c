#include "sysinit.h"
#include "chip.h"
#include "CANlib.h"

#include <string.h>

#define SERIAL_BAUDRATE 57600
#define CAN_BAUDRATE 500000

#define DRIVER_RESET_PIN_AND_PORT 1, 1
#define RTD_PIN_AND_PORT 1, 2
#define SCROLL_SELECT_PIN_AND_PORT 1, 3

#define DRIVER_RESET_PIN_IOCON IOCON_PIO1_1
#define RTD_PIN_PIN_IOCON IOCON_PIO1_2
#define SCROLL_SELECT_PIN_IOCON IOCON_PIO1_3

#define PIN_CONFIG IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_PULLDOWN // Fix pull

// pulldown or pullup?
#define BUTTON_DOWN true

Can_ErrorID_T write_can_driver_reset(bool state);
Can_ErrorID_T write_can_rtd(bool state);
Can_ErrorID_T write_can_scroll_select(bool state);

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

void SysTick_Handler(void) {
  msTicks++;
}

void Serial_Init(uint32_t baudrate) {
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* TXD */

    Chip_UART_Init(LPC_USART);
    Chip_UART_SetBaud(LPC_USART, baudrate);
    // Configure data width, parity, and stop bits
    Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
    Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(LPC_USART);
}

void print(const char* str) {
    Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}

void GPIO_Init(void) {
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, DRIVER_RESET_PIN_IOCON);
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, RTD_PIN_PIN_IOCON);
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, SCROLL_SELECT_PIN_IOCON);

    Chip_IOCON_PinMuxSet(LPC_IOCON, DRIVER_RESET_PIN_IOCON, PIN_CONFIG);
    Chip_IOCON_PinMuxSet(LPC_IOCON, RTD_PIN_PIN_IOCON, PIN_CONFIG);
    Chip_IOCON_PinMuxSet(LPC_IOCON, SCROLL_SELECT_PIN_IOCON, PIN_CONFIG);

    // Configure pins as inputs
    // Chip_GPIO_SetPinDIR(LPC_GPIO, DRIVER_RESET_PIN_AND_PORT, false);
    // Chip_GPIO_SetPinDIR(LPC_GPIO, RTD_PIN_AND_PORT, false);
    // Chip_GPIO_SetPinDIR(LPC_GPIO, SCROLL_SELECT_PIN_AND_PORT, false);

    Chip_GPIO_Init(LPC_GPIO);
}

bool read_pin(uint32_t port, uint8_t bit) {
  return Chip_GPIO_GetPinState(LPC_GPIO, port, bit);
}

void handle_can_error(Can_ErrorID_T error) {
  if (error != Can_Error_NONE && error != Can_Error_NO_RX) { // Unimorportant errors
    switch(error) {
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
        break;
      case Can_Error_RX_BUFFER_FULL:
        print("Can_Error_RX_BUFFER_FULL\n");
        break;
    }
  }
}

typedef struct button_states {
    bool rtd;
    bool driver_reset;
    // need to name other buttons
} button_states_t;

button_states_t poll_buttons(void) {
    button_states_t bs;
    bs.rtd          = (read_pin(RTD_PIN_AND_PORT)          == BUTTON_DOWN);
    bs.driver_reset = (read_pin(DRIVER_RESET_PIN_AND_PORT) == BUTTON_DOWN);
    return bs;
}

int main(void) {
    SystemCoreClockUpdate();

    Serial_Init(SERIAL_BAUDRATE);
    Can_Init(CAN_BAUDRATE);

    if (SysTick_Config (SystemCoreClock / 1000)) {
        while(1); // error
    }

    uint32_t timer = 0;

    button_states_t hold = {false, false};
    while(1) {
        button_states_t current = poll_buttons();
        hold.rtd          |= current.rtd;
        hold.driver_reset |= current.driver_reset;

        if (msTicks > timer) {
            timer = msTicks + can0_ButtonRequest_period;

            can0_ButtonRequest_T msg;
            msg.RTD         = hold.rtd;
            msg.DriverReset = hold.driver_reset;

            hold.rtd          = false;
            hold.driver_reset = false;

            can0_ButtonRequest_Write(&msg);
        }
    }

    return 0;
}
