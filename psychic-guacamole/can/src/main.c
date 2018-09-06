#include "chip.h"
#include <string.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define TEST_CCAN_BAUD_RATE 500000

#define LED0 2, 8
#define LED1 2, 10

#define BUFFER_SIZE 8
#define UART_RX_BUFFER_SIZE 8

const uint32_t OscRateIn = 12000000;


volatile uint32_t millis = 0;


CCAN_MSG_OBJ_T msg_obj;
STATIC RINGBUFF_T rx_buffer;
CCAN_MSG_OBJ_T    _rx_buffer[8];
static bool can_error_flag;
static uint32_t can_error_info;

static char str[100];
static char uart_rx_buf[UART_RX_BUFFER_SIZE];

#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
        # define DEBUG_Print(str) Chip_UART_SendBlocking(LPC_USART, str, \
                                                         strlen(str))
        # define DEBUG_Write(str, count) Chip_UART_SendBlocking(LPC_USART, \
                                                                str,       \
                                                                count)
#else  /* ifdef DEBUG_ENABLE */
        # define DEBUG_Print(str)
        # define DEBUG_Write(str, count)
#endif /* ifdef DEBUG_ENABLE */

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void SysTick_Handler(void) {
  millis++;
}

static void LED_Init(uint8_t port, uint8_t pin) {
  Chip_GPIO_WriteDirBit(LPC_GPIO, port, pin, true);
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, false);
}

static void LED_Write(uint8_t port, uint8_t pin, uint8_t val) {
  Chip_GPIO_SetPinState(LPC_GPIO, port, pin, val);
}

void baudrateCalculate(uint32_t baud_rate, uint32_t *can_api_timing_cfg)
{
  uint32_t pClk, div, quanta, segs, seg1, seg2, clk_per_bit, can_sjw;

  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
  pClk = Chip_Clock_GetMainClockRate();

  clk_per_bit = pClk / baud_rate;

  for (div = 0; div <= 15; div++) {
    for (quanta = 1; quanta <= 32; quanta++) {
      for (segs = 3; segs <= 17; segs++) {
        if (clk_per_bit == (segs * quanta * (div + 1))) {
          segs                 -= 3;
          seg1                  = segs / 2;
          seg2                  = segs - seg1;
          can_sjw               = seg1 > 3 ? 3 : seg1;
          can_api_timing_cfg[0] = div;
          can_api_timing_cfg[1] =
            ((quanta - 1) & 0x3F) | (can_sjw & 0x03) << 6 | (seg1 & 0x0F) << 8 |
            (seg2 & 0x07) << 12;
          return;
        }
      }
    }
  }
}

/*	CAN receive callback */

/*	Function is executed by the Callback handler after
    a CAN message has been received */
void CAN_rx(uint8_t msg_obj_num) {
  /* Determine which CAN message has been received */
  msg_obj.msgobj = msg_obj_num;

  /* Now load up the msg_obj structure with the CAN message */
  LPC_CCAN_API->can_receive(&msg_obj);

  if (msg_obj_num == 1) {
    // TODO: Insert message object into ring buffer rx_buffer
  }
}

/*	CAN transmit callback */

/*	Function is executed by the Callback handler after
    a CAN message has been transmitted */
void CAN_tx(uint8_t msg_obj_num) {}

/*	CAN error callback */

/*	Function is executed by the Callback handler after
    an error has occurred on the CAN bus */
void CAN_error(uint32_t error_info) {
  can_error_info = error_info;
  can_error_flag = true;
}

/**
 * @brief	CCAN Interrupt Handler
 * @return	Nothing
 * @note	The CCAN interrupt handler must be provided by the user
 *application.
 *	It's function is to call the isr() API located in the ROM
 */
void CAN_IRQHandler(void) {
  LPC_CCAN_API->isr();
}

int main(void) {
  SystemCoreClockUpdate();

  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Error
    while (1);
  }

  // ---------------
  // UART

	// RXD
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));
	// TXD
  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));

  Chip_UART_Init(LPC_USART);
  Chip_UART_SetBaud(LPC_USART, 9600);
  Chip_UART_ConfigData(LPC_USART,
                       (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT |
                        UART_LCR_PARITY_DIS));
  Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
  Chip_UART_TXEnable(LPC_USART);

  // ---------------

  DEBUG_Print("Started up\n\r");

  LED_Init(LED0);
  LED_Init(LED1);

  LED_Write(LED0, true);

  // ---------------
  // Ring Buffer

  RingBuffer_Init(&rx_buffer, _rx_buffer, sizeof(CCAN_MSG_OBJ_T), 8);
  RingBuffer_Flush(&rx_buffer);

  // ---------------

  uint32_t CanApiClkInitTable[2];

  /* Publish CAN Callback Functions */
  CCAN_CALLBACKS_T callbacks = {
    CAN_rx,
    CAN_tx,
    CAN_error,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
  };
  baudrateCalculate(TEST_CCAN_BAUD_RATE, CanApiClkInitTable);

  LPC_CCAN_API->init_can(&CanApiClkInitTable[0], TRUE);

  /* Configure the CAN callback functions */
  LPC_CCAN_API->config_calb(&callbacks);

  /* Enable the CAN Interrupt */
  NVIC_EnableIRQ(CAN_IRQn);

  // typedef struct CCAN_MSG_OBJ {
  //    uint32_t  mode_id;
  //    uint32_t  mask;
  //    uint8_t   data[8];
  //    uint8_t   dlc;
  //    uint8_t   msgobj;
  // } CCAN_MSG_OBJ_T;

  /* Configure message object 1 to only ID 0x600 */

  // THIS IS MSG OBJ ID FOR READING MESSAGES
  msg_obj.msgobj  = 1;
  msg_obj.mode_id = 0x600;
  msg_obj.mask    = 0x7FF;
  LPC_CCAN_API->config_rxmsgobj(&msg_obj);


  can_error_flag = false;
  can_error_info = 0;

  while (1) {
    if (!RingBuffer_IsEmpty(&rx_buffer)) {
      CCAN_MSG_OBJ_T temp_msg;

      // TODO Pop from ring buffer into CCAN_MSG_OBJ
      DEBUG_Print("Received Message ID: 0x");
      itoa(9999999, str, 16); // replace with message ID of temp_msg
      DEBUG_Print(str);
      DEBUG_Print("\r\n");
    }

    if (can_error_flag) {
      can_error_flag = false;
      DEBUG_Print("CAN Error: 0b");
      itoa(can_error_info, str, 2);
      DEBUG_Print(str);
      DEBUG_Print("\r\n");
    }

		// Returns the number of bytes (characters) in the buffer now
    uint8_t count = Chip_UART_Read(LPC_USART, uart_rx_buf, UART_RX_BUFFER_SIZE);

    if (count != 0) {
      // when character 'a' is recieved, send a can message object with the
      // following specs:
      //      - msgobj memory slot 2
      //      - CAN ID 0x700
      //      - one byte of data, 0xAA or whatever you choose
      //
      // Some thoughts and code to help you get along:
      // switch (uart_rx_buf[0]) {
      // ....
      // ....
      // LPC_CCAN_API->can_transmit(&msg_obj);
      // break;
      // ...
      // }
    }
  }
}
