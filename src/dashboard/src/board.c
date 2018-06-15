#include "board.h"

const uint32_t OscRateIn = 12000000;
volatile uint32_t msTicks;

void SysTick_Handler(void) {
    msTicks++;
}

void Delay(uint32_t dly) {
    uint32_t stop = msTicks + dly;
    while (msTicks < stop);
}

void Board_Chip_Init(void) {
    SysTick_Config(SystemCoreClock / 1000);
}

void Pin_Init_Out(uint8_t port, uint8_t pin) {
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
    Chip_GPIO_SetPinState(LPC_GPIO, port, pin, false); // low
}

void Pin_Init_In(uint8_t port, uint8_t pin) {
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
}

void Pin_Write(uint8_t port, uint8_t pin, uint8_t val) {
    Chip_GPIO_SetPinState(LPC_GPIO, port, pin, val);
}

bool Pin_Read(uint8_t port, uint8_t pin) {
    return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
}

uint16_t ADC_Read(ADC_CHANNEL_T ch) {
    uint16_t v;
    Chip_ADC_ReadValue(LPC_ADC, ch, &v);
    return v;
}

void Board_GPIO_Init(void) {
    Chip_GPIO_Init(LPC_GPIO);

    Pin_Init_Out(PIN_OLED_RS);
    Pin_Init_Out(PIN_OLED_CS);
    Pin_Init_Out(PIN_OLED_SCLK);
    Pin_Init_Out(PIN_OLED_SDI);
    Pin_Init_Out(PIN_OLED_SDO);

    Pin_Init_Out(PIN_LED_HV);
    Pin_Init_Out(PIN_LED_RTD);
    Pin_Init_Out(PIN_LED_IMD);
    Pin_Init_Out(PIN_LED_AMS);

    Pin_Init_In(PIN_BUTTON_LEFT);
    Pin_Init_In(PIN_BUTTON_RIGHT);

    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIN_OLED_SCLK, IOCON_FUNC1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIN_OLED_SDI,  IOCON_FUNC1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIN_OLED_SDO,  IOCON_FUNC1);

    // ADC stuff
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_KNOB, IOCON_FUNC2 | IOCON_ADMODE_EN);
    Chip_ADC_EnableChannel(LPC_ADC, KNOB_CHANNEL, ENABLE);
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
