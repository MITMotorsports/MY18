#include "board.h"
#include "ltc6804.h"
#include "CANlib.h"
#include "cell_temperatures.h"
#include "error_handler.h"
#include "chip.h"

#define TEST_HARDWARE

#define BMS_CONTACTOR_WELD_PERIOD 1000
#define BMS_ERRORS_PERIOD 1000
#define BMS_SOC_PERIOD 1000
#define BMS_PACK_STATUS_PERIOD 1000

static uint32_t last_bms_pack_status_time = 0;
static uint32_t last_bms_contactor_weld_time = 0;
static uint32_t last_bms_soc_time = 0;
static uint32_t last_bms_errors_time = 0;

volatile uint32_t msTicks;

const uint32_t OscRateIn = 0;

static RINGBUFF_T uart_rx_ring;
static uint8_t _uart_rx_ring[UART_BUFFER_SIZE];
static RINGBUFF_T uart_tx_ring;
static uint8_t _uart_tx_ring[UART_BUFFER_SIZE];

static LTC6804_CONFIG_T ltc6804_config;
static LTC6804_STATE_T ltc6804_state;
static Chip_SSP_DATA_SETUP_T ltc6804_xf_setup;
static uint8_t ltc6804_tx_buf[LTC6804_CALC_BUFFER_LEN(MAX_NUM_MODULES)];
static uint8_t ltc6804_rx_buf[LTC6804_CALC_BUFFER_LEN(MAX_NUM_MODULES)];
static uint8_t ltc6804_cfg[LTC6804_DATA_LEN];
static uint16_t ltc6804_bal_list[MAX_NUM_MODULES];
static LTC6804_ADC_RES_T ltc6804_adc_res;
static LTC6804_OWT_RES_T ltc6804_owt_res;

// ltc6804 timing variables
static bool _ltc6804_gcv;
static uint32_t _ltc6804_last_gcv;
static uint32_t _ltc6804_gcv_tick_time;
static bool _ltc6804_owt;
static uint32_t _ltc6804_last_owt;
static uint32_t _ltc6804_owt_tick_time;
static uint32_t board_lastThermistorShiftTime_ms = 0;

uint8_t currentThermistor = 0;
static bool ltc6804_setMultiplexerAddressFlag = false;
static bool ltc6804_getThermistorVoltagesFlag = false;

static bool _ltc6804_initialized;
static LTC6804_INIT_STATE_T _ltc6804_init_state;

static char str[10];




static bool isResetting = false;


static ADC_CLOCK_SETUP_T adc_setup;

void UART_IRQHandler(void) {
    Chip_UART_IRQRBHandler(LPC_USART, &uart_rx_ring, &uart_tx_ring);
}

void SysTick_Handler(void) {
	msTicks++;
}

void Board_Chip_Init(void) {
	SysTick_Config(Hertz2Ticks(1000));
}

uint32_t Board_Print(const char *str) {
    return Chip_UART_SendRB(LPC_USART, &uart_tx_ring, str, strlen(str));
}

void Board_BlockingDelay(uint32_t dlyTicks) {
    uint32_t curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}
uint32_t Board_Println(const char *str) {
    uint32_t count = Board_Print(str);
    return count + Board_Print("\r\n");
}

uint32_t Board_PrintNum(uint32_t a, uint8_t base) {
    itoa(a, str, base);
    return Board_Println_BLOCKING(str);
}

uint32_t Board_Write(const char *str, uint32_t count) {
    return Chip_UART_SendRB(LPC_USART, &uart_tx_ring, str, count);
}

uint32_t Board_Read(char *charBuffer, uint32_t length) {
    return Chip_UART_ReadRB(LPC_USART, &uart_rx_ring, charBuffer, length);
}

uint32_t Board_Print_BLOCKING(const char *str) {
    return Chip_UART_SendBlocking(LPC_USART, str, strlen(str));
}
uint32_t Board_Println_BLOCKING(const char *str) {
    uint32_t count = Board_Print_BLOCKING(str);
    return count + Board_Print_BLOCKING("\r\n");
}

void Board_GPIO_Init(void) {
    Chip_GPIO_Init(LPC_GPIO);

    //LED1
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED1, IOCON_LED1_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED1, false);

    //LED2
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED2);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED2, IOCON_LED2_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED2, false);

    //LED3
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LED3);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LED3, IOCON_LED3_FUNC);
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_LED3, false);

    //SSP for EEPROM
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* MISO1 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* MOSI1 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* SCK1 */

    //SSP for LTC6804
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC1 | IOCON_MODE_PULLUP));   /* MISO0 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC1 | IOCON_MODE_PULLUP));   /* MOSI0 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC2 | IOCON_MODE_INACT));    /* SCK0 */
    Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO0_6);

    //is the cs pin supposed to be initialized
    //Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 | IOCON_MODE_INACT));

    //FSAE specific pin intializations

    //Contactor Weld
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, PIN_CONTACTOR_WELD_1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CONTACTOR_WELD_1, (IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_PULLUP));

    Chip_GPIO_SetPinDIRInput(LPC_GPIO, PIN_CONTACTOR_WELD_2);
    // Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CONTACTOR_WELD_2, (IOCON_FUNC1 | IOCON_DIGMODE_EN | IOCON_MODE_PULLUP));
    //for analog mode
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CONTACTOR_WELD_2, (IOCON_FUNC2 | IOCON_ADMODE_EN));

    //High Side Detect
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_HIGH_SIDE_DETECT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_HIGH_SIDE_DETECT, (IOCON_FUNC0 | IOCON_MODE_INACT));

    //Low Side Detect
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_LOW_SIDE_DETECT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_LOW_SIDE_DETECT, (IOCON_FUNC0 | IOCON_MODE_INACT));

    //Charge Enable Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_CHARGER_ENABLE);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_CHARGER_ENABLE,
            (IOCON_FUNC0 | IOCON_MODE_INACT));

    //Fault Pin
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_BMS_FAULT);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_BMS_FAULT,
            (IOCON_FUNC0 | IOCON_DIGMODE_EN | IOCON_MODE_PULLDOWN));

    //Enable pull down resistors on unused pins
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_23);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_23, (IOCON_PIN_23_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_23, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_29);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_29, (IOCON_PIN_29_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_29, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_32);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_32, (IOCON_PIN_32_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_32, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_33);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_33, (IOCON_PIN_33_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_33, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_9);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_9, (IOCON_PIN_9_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_9, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_17);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_17, (IOCON_PIN_17_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_17, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_2);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_2, (IOCON_PIN_2_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_2, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_21);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_21, (IOCON_PIN_21_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_21, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_1);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_1, (IOCON_PIN_1_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_1, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_11);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_11, (IOCON_PIN_11_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_11, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_12);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_12, (IOCON_PIN_12_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_12, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_25);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_25, (IOCON_PIN_25_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_25, false);

    Chip_GPIO_SetPinDIROutput(LPC_GPIO, PIN_48);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PIN_IOCON_48, (IOCON_PIN_48_FUNC | IOCON_MODE_PULLDOWN));
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_48, false);
}
void Board_CAN_Init() {
    init_can0_bms();
}

void Board_UART_Init(uint32_t baudRateHz){
    RingBuffer_Init(&uart_rx_ring, _uart_rx_ring, sizeof(uint8_t), UART_BUFFER_SIZE);
    RingBuffer_Flush(&uart_rx_ring);
    RingBuffer_Init(&uart_tx_ring, _uart_tx_ring, sizeof(uint8_t), UART_BUFFER_SIZE);
    RingBuffer_Flush(&uart_tx_ring);

    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));/* RXD */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));/* TXD */

    Chip_UART_Init(LPC_USART);
    Chip_UART_SetBaudFDR(LPC_USART, baudRateHz);
    Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
    Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(LPC_USART);

    Chip_UART_IntEnable(LPC_USART, UART_IER_RBRINT);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

}

void Board_LED_On(uint8_t led_gpio, uint8_t led_pin) {
    Chip_GPIO_SetPinOutHigh(LPC_GPIO, led_gpio, led_pin);
}

void Board_LED_Off(uint8_t led_gpio, uint8_t led_pin) {
    Chip_GPIO_SetPinOutLow(LPC_GPIO, led_gpio, led_pin);
}

void Board_LED_Toggle(uint8_t led_gpio, uint8_t led_pin) {
    Chip_GPIO_SetPinState(LPC_GPIO, led_gpio, led_pin,
        1 - Chip_GPIO_GetPinState(LPC_GPIO, led_gpio, led_pin));
}

void Board_Contactors_Set(bool close_contactors) {
    Chip_GPIO_SetPinState(LPC_GPIO, PIN_BMS_FAULT, close_contactors);
}

bool Board_Contactors_Closed(void) {
    return Chip_GPIO_GetPinState(LPC_GPIO, PIN_BMS_FAULT);
}
//Analog
void Board_ADC_Init() {
    Chip_ADC_Init(LPC_ADC, &adc_setup);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH4, ENABLE);
    Chip_ADC_SetBurstCmd(LPC_ADC, 1);
    Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

}/*

void Board_Contactor_Weld_One(int16_t* adc_data) {
    while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH5, ADC_DR_DONE_STAT)) {}
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH5, adc_data);

}*/
bool Board_Contactor_Two_Welded() {
    int16_t adc_data;
    while (!Chip_ADC_ReadStatus(LPC_ADC, ADC_CH4, ADC_DR_DONE_STAT)) {}
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH4, &adc_data);
    return (adc_data<800);
}

//Digital
bool Board_Contactor_One_Welded(void) {
    //returns False when shorted

    return Chip_GPIO_GetPinState(LPC_GPIO, PIN_CONTACTOR_WELD_1);
}
// bool Board_Contactor_Two_Welded(void) {
//     if(Chip_GPIO_GetPinState(LPC_GPIO, PIN_CONTACTOR_WELD_2) == true) {
//         Board_Println_BLOCKING("2 is true");
//     } else {
//         Board_Println_BLOCKING("2 is false");
//     }
//     return Chip_GPIO_GetPinState(LPC_GPIO, PIN_CONTACTOR_WELD_2);
// }


bool Board_LTC6804_CVST() {
#ifdef TEST_HARDWARE_LTC_TEST
    return true;
#else
    LTC6804_STATUS_T res;
    uint32_t count = msTicks;
    // while(msTicks - count < 2000) {
    //     Board_PrintNum(msTicks,10);
    // }
    res = LTC6804_CVST(&ltc6804_config, &ltc6804_state, msTicks);

    switch (res) {
        case LTC6804_FAIL:
            Board_Println("CVST FAIL");
            Error_Assert(ERROR_LTC6804_CVST, msTicks);
            return false;
        case LTC6804_PEC_ERROR:
            Board_Println("CVST PEC_ERROR");
            Error_Assert(ERROR_LTC6804_PEC, msTicks);
            return false;
        case LTC6804_PASS:
            Board_Println("CVST PASS");
            Error_Pass(ERROR_LTC6804_CVST);
            return true;
        case LTC6804_WAITING:
        case LTC6804_WAITING_REFUP:
            return false;
        default:
            Board_Println("WTF");
            return false;
    }

    return false;
#endif
}

void Board_LTC6804_UpdateBalanceStates(bool *balance_req) {
#ifdef TEST_HARDWARE
    UNUSED(balance_req);
    return;
#else
    LTC6804_UpdateBalanceStates(&ltc6804_config, &ltc6804_state, balance_req, msTicks);
#endif
}

bool Board_LTC6804_OpenWireTest(void) {
#ifdef TEST_HARDWARE_LTC_TEST
    return true; // Change to simulate during test
#else

    if (msTicks - _ltc6804_last_owt > _ltc6804_owt_tick_time) {
        _ltc6804_owt = true;
    }

    if (!_ltc6804_owt) {
        return false;
    }

    LTC6804_STATUS_T res;
    res = LTC6804_OpenWireTest(&ltc6804_config, &ltc6804_state, &ltc6804_owt_res, msTicks);

    switch (res) {
        case LTC6804_FAIL:
            Board_Print("OWT FAIL, mod=");
            itoa(ltc6804_owt_res.failed_module, str, 10);
            Board_Print(str);
            Board_Print(" wire=");
            itoa(ltc6804_owt_res.failed_wire, str, 10);
            Board_Println(str);
            Error_Assert(ERROR_LTC6804_OWT, msTicks);
            return false;
        case LTC6804_PEC_ERROR:
            Board_Println("OWT PEC_ERROR");
            Error_Assert(ERROR_LTC6804_PEC,msTicks);
            return false;
        case LTC6804_PASS:
            Board_Println("OWT PASS");
            _ltc6804_owt = false;
            _ltc6804_last_owt = msTicks;
            Error_Pass(ERROR_LTC6804_OWT);
            return true;
        case LTC6804_WAITING:
        case LTC6804_WAITING_REFUP:
            // Board_Println("*");
            return false;
        default:
            Board_Println("WTF");
            return false;
    }
#endif
}
bool Board_LTC6804_Init(BMS_PACK_CONFIG_T *pack_config, uint32_t *cell_voltages_mV){
#ifdef TEST_HARDWARE_LTC_TEST
    UNUSED(pack_config); UNUSED(cell_voltages_mV);
    return true;
#else
    if (_ltc6804_initialized) return true;

    if (_ltc6804_init_state == LTC6804_INIT_NONE) {
        ltc6804_config.pSSP = LPC_SSP0;
        ltc6804_config.baud = LTC6804_BAUD;
        ltc6804_config.cs_gpio = 0;
        ltc6804_config.cs_pin = 2;

        ltc6804_config.num_modules = pack_config->num_modules;
        ltc6804_config.module_cell_count = pack_config->module_cell_count;

        ltc6804_config.min_cell_mV = pack_config->cell_min_mV;
        ltc6804_config.max_cell_mV = pack_config->cell_max_mV;

        ltc6804_config.adc_mode = LTC6804_ADC_MODE_NORMAL;

        ltc6804_state.xf = &ltc6804_xf_setup;
        ltc6804_state.tx_buf = ltc6804_tx_buf;
        ltc6804_state.rx_buf = ltc6804_rx_buf;
        ltc6804_state.cfg = ltc6804_cfg;
        ltc6804_state.bal_list = ltc6804_bal_list;

        ltc6804_adc_res.cell_voltages_mV = cell_voltages_mV;

        ltc6804_owt_res.failed_wire = 0;
        ltc6804_owt_res.failed_module = 0;

        _ltc6804_gcv = false;
        _ltc6804_last_gcv = 0;
        _ltc6804_gcv_tick_time = 100;
        _ltc6804_owt = true;
        _ltc6804_last_owt = 0;
        _ltc6804_owt_tick_time = 60000;



        LTC6804_Init(&ltc6804_config, &ltc6804_state, msTicks);

        _ltc6804_init_state = LTC6804_INIT_CFG;
    } else if (_ltc6804_init_state == LTC6804_INIT_CFG) {
        //Board_Println("CFG");
        bool res = Board_LTC6804_CVST();
        if (res) {
            //Board_Println("CVST passed");
            _ltc6804_init_state = LTC6804_INIT_CVST;
        }
    } else if (_ltc6804_init_state == LTC6804_INIT_CVST) {
        //Board_Println("CVST state");
        bool res = Board_LTC6804_OpenWireTest();
        if (res) {
          //  Board_Println("OWT passed");
            _ltc6804_init_state = LTC6804_INIT_DONE;
        }
    } else if (_ltc6804_init_state == LTC6804_INIT_DONE) {
        //Board_Println("ltc init done");
        _ltc6804_initialized = true;
        _ltc6804_init_state = 0;
        return true;
    }
    return false;
#endif
}

void Board_LTC6804_DeInit(void) {
#ifndef TEST_HARDWARE
    _ltc6804_initialized = false;
    _ltc6804_init_state = LTC6804_INIT_NONE;
#endif
}

void Board_GetModeRequest(const CONSOLE_OUTPUT_T *console_output, BMS_INPUT_T* bms_input) {

    //create console mode request
    BMS_SSM_MODE_T console_mode_request = BMS_SSM_MODE_STANDBY;

    //then get console mode request
    if(console_output->valid_mode_request) {
        console_mode_request = console_output->mode_request;
    }
    if(console_mode_request == BMS_SSM_MODE_STANDBY && bms_input->csb_mode_request == BMS_SSM_MODE_STANDBY) {
        bms_input->mode_request = bms_input->vcu_mode_request;
    }
    //if console mode request is standby vcu mode request is standby:
        //mode request is charger mode request
        //Error pass conflicting mode requests
    //else if (charge mode request is standby and vcu mode requst is stadndby):
        //mode request is console mode request
        //Error pass conflicting mode requests
    //else if console mode request is standby and charge mode reuqest is standby
        //mode request is vcu mode request
    //else if console mode request == discharge mode request or cosole mode request == charge mode request
        //mode request is console
}
void Board_LTC6804_ProcessInputs(BMS_PACK_STATUS_T *pack_status, BMS_STATE_T* bms_state) {
    Board_LTC6804_GetCellVoltages(pack_status);
    Board_LTC6804_GetCellTemperatures(pack_status, bms_state->pack_config->num_modules);
    Board_LTC6804_OpenWireTest();
}

void Board_LTC6804_GetCellVoltages(BMS_PACK_STATUS_T *pack_status){
#ifdef TEST_HARDWARE_LTC_TEST
    UNUSED(pack_status);
    return;
#else
    Board_Println("In get cell voltages");
    if (msTicks - _ltc6804_last_gcv > _ltc6804_gcv_tick_time) {
        _ltc6804_gcv = true;
    }

    if (!_ltc6804_gcv) {
        return;
    }

    LTC6804_STATUS_T res = LTC6804_GetCellVoltages(&ltc6804_config, &ltc6804_state, &ltc6804_adc_res, msTicks);
    switch (res) {
        case LTC6804_FAIL:
            Board_Println("Get Vol FAIL");
            break;
        case LTC6804_PEC_ERROR:
            Board_Println_BLOCKING("Get Vol PEC_ERROR");
            Error_Assert(ERROR_LTC6804_PEC,msTicks);
            break;
        case LTC6804_PASS:
            Board_Println_BLOCKING("Get Vol Success");
            pack_status->pack_cell_min_mV = ltc6804_adc_res.pack_cell_min_mV;
            pack_status->pack_cell_max_mV = ltc6804_adc_res.pack_cell_max_mV;
            LTC6804_ClearCellVoltages(&ltc6804_config, &ltc6804_state, msTicks); // [TODO] Use this to your advantage
            _ltc6804_gcv = false;
            _ltc6804_last_gcv = msTicks;
            Error_Pass(ERROR_LTC6804_PEC);
        case LTC6804_WAITING:
        case LTC6804_WAITING_REFUP:
            break;
        default:
            Board_Println("WTF");
    }
#endif
}

void Board_LTC6804_GetCellTemperatures(BMS_PACK_STATUS_T * pack_status, uint8_t num_modules) {
#ifndef TEST_HARDWARE_LTC_TEST
    Board_Println("In get cell temperatures");
    if ((msTicks - board_lastThermistorShiftTime_ms) > TIME_PER_THERMISTOR_MS) {
        board_lastThermistorShiftTime_ms = msTicks;

        // if we finished reading previous thermistor voltage, go to next thermistor
        if (!ltc6804_setMultiplexerAddressFlag && !ltc6804_getThermistorVoltagesFlag) {
            if (currentThermistor < (MAX_THERMISTORS_PER_MODULE-1)) {
                currentThermistor += 1;
            } else {
                currentThermistor = 0;
            }

            // set flags to true
            ltc6804_setMultiplexerAddressFlag = true;
            ltc6804_getThermistorVoltagesFlag = true;
        }

    }
     LTC6804_STATUS_T status;

    // set multiplexer address
    // if flag is not true, skip this step
    if (ltc6804_setMultiplexerAddressFlag) {

        // initalize CLOCK and LATCH input to the shift register
        status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                LTC6804_SHIFT_REGISTER_CLOCK, 0, msTicks);
        Board_HandleLtc6804Status(status);
        if (status != LTC6804_PASS) return;

        status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                LTC6804_SHIFT_REGISTER_LATCH, 0, msTicks);
        Board_HandleLtc6804Status(status);
        if (status != LTC6804_PASS) return;

        // Get thermistor address
        uint8_t thermistorAddress = 0;
        if (currentThermistor <= THERMISTOR_GROUP_ONE_END) {
            thermistorAddress = currentThermistor + THERMISTOR_GROUP_ONE_OFFSET;
        } else if ((THERMISTOR_GROUP_TWO_START <= currentThermistor) &&
                (currentThermistor <= THERMISTOR_GROUP_TWO_END)) {
            thermistorAddress = currentThermistor + THERMISTOR_GROUP_TWO_OFFSET;
        } else if ((THERMISTOR_GROUP_THREE_START <= currentThermistor) &&
                (currentThermistor <= THERMISTOR_GROUP_THREE_END)) {
            thermistorAddress = currentThermistor + THERMISTOR_GROUP_THREE_OFFSET;
        } else {
            Board_Println("Invalid value of currentThermistor. You should never reach here");
            Error_Assert(ERROR_CONTROL_FLOW, msTicks);
        }

        // shift bits into shift resgister
        int8_t i;
        for (i=7; i>=0; i--) {
            uint8_t addressBit = (thermistorAddress & (1<<i) ) >> i;
            status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                    LTC6804_SHIFT_REGISTER_DATA_IN, addressBit, msTicks);
            Board_HandleLtc6804Status(status);
            if (status != LTC6804_PASS) return;

            status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                    LTC6804_SHIFT_REGISTER_CLOCK, 1, msTicks);
            Board_HandleLtc6804Status(status);
            if (status != LTC6804_PASS) return;

            status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                    LTC6804_SHIFT_REGISTER_CLOCK, 0, msTicks);
            Board_HandleLtc6804Status(status);
            if (status != LTC6804_PASS) return;

        }

        // Latch the outputs
        status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                LTC6804_SHIFT_REGISTER_LATCH, 1, msTicks);
        Board_HandleLtc6804Status(status);
        if (status != LTC6804_PASS) return;

        status = LTC6804_SetGPIOState(&ltc6804_config, &ltc6804_state,
                LTC6804_SHIFT_REGISTER_LATCH, 0, msTicks);
        Board_HandleLtc6804Status(status);
        if (status != LTC6804_PASS) return;

        // Finished setting multiplexer address. Reset flag
        ltc6804_setMultiplexerAddressFlag = false;

    }

    // Get thermistor voltages
    // if flag is not true, return
    if (!ltc6804_getThermistorVoltagesFlag) {
        return;
    }

    uint32_t gpioVoltages[MAX_NUM_MODULES * LTC6804_GPIO_COUNT];
    status = LTC6804_GetGPIOVoltages(&ltc6804_config, &ltc6804_state, gpioVoltages,
            msTicks);
    Board_HandleLtc6804Status(status);
    if (status != LTC6804_PASS) return;

    CellTemperatures_UpdateCellTemperaturesArray(gpioVoltages, currentThermistor,
            pack_status, num_modules);

    // Finished getting thermistor voltages. Reset flag
    ltc6804_getThermistorVoltagesFlag = false;

    if (currentThermistor == THERMISTOR_GROUP_THREE_END) {
        CellTemperatures_UpdateMaxMinAvgCellTemperatures(pack_status, num_modules);
    }


#else
    UNUSED(pack_status);
    UNUSED(num_modules);
#endif //TEST_HARDWARE
}

#ifndef TEST_HARDWARE_LTC_TEST
void Board_HandleLtc6804Status(LTC6804_STATUS_T status) {

    switch (status) {
        case LTC6804_WAITING:
            break;
        case LTC6804_PASS:
            Error_Pass(ERROR_LTC6804_PEC);
            break;
        case LTC6804_FAIL:
            Board_Println("LTC6804 fail");
            break;
        case LTC6804_PEC_ERROR:
            Board_Println("LTC6804 PEC_ERROR");
            Error_Assert(ERROR_LTC6804_PEC, msTicks);
        case LTC6804_WAITING_REFUP:
            break;
        default:
            Board_Println("Entered default case in Board_HandleLtc6804Status(). You should never reach here");
            Error_Assert(ERROR_CONTROL_FLOW, msTicks);


    }
}
#endif //TEST_HARDWARE

Frame can_input;
void Board_CAN_Receive(BMS_INPUT_T *bms_input){
    Can_RawRead(&can_input);
    can0_T msgForm = identify_can0(&can_input);
    switch(msgForm) {
        case can0_BMSState:
            can_receive_bms_state(bms_input);
            break;
        case can0_BmsVcuSwitch:
            can_receive_vcu_switch(bms_input);
            break;
        case can0_CurrentSensor_Current:
            can_receive_current(bms_input);
            break;
        case can0_CurrentSensor_Voltage:
            can_receive_voltage(bms_input);
            break;
        case can0_CurrentSensor_Energy:
            can_receive_energy(bms_input);
            break;
        case CAN_UNKNOWN_MSG:
            break;
        default:
            break;
    }
}

void can_receive_bms_state(BMS_INPUT_T *bms_input) {
    can0_BMSState_T msg;
    unpack_can0_BMSState(&can_input,&msg);
    bms_input->vcu_mode_request = msg.state;
}

void can_receive_vcu_switch(BMS_INPUT_T *bms_input) {
    can0_BmsVcuSwitch_T msg;
    unpack_can0_BmsVcuSwitch(&can_input,&msg);
    bms_input->vcu_switch = msg.always_true;
}

void can_receive_current(BMS_INPUT_T *bms_input) {
    can0_CurrentSensor_Current_T msg;
    unpack_can0_CurrentSensor_Current(&can_input, &msg);
    bms_input->pack_status->pack_current_mA = msg.pack_current > 0 ? msg.pack_current : -msg.pack_current;
}

void can_receive_voltage(BMS_INPUT_T *bms_input) {
    can0_CurrentSensor_Voltage_T msg;
    unpack_can0_CurrentSensor_Voltage(&can_input, &msg);
    bms_input->pack_status->pack_voltage_mV = msg.pack_voltage > 0 ? msg.pack_voltage : -msg.pack_voltage;
}

void can_receive_energy(BMS_INPUT_T *bms_input) {
    can0_CurrentSensor_Energy_T msg;
    unpack_can0_CurrentSensor_Energy(&can_input, &msg);
    bms_input->pack_status->pack_energy_wH = msg.pack_energy;  
}

Frame can_output;
void Board_CAN_Transmit(BMS_INPUT_T *bms_input, BMS_OUTPUT_T *bms_output){

    uint32_t msTicks = bms_input->msTicks;
    can_transmit_contactor_weld(bms_input, msTicks);
    can_transmit_pack_status(bms_input, msTicks);
    can_transmit_bms_soc(bms_input, msTicks);
    //can_transmit_bms_errors(bms_input, msTicks);

}

void can_transmit_contactor_weld(BMS_INPUT_T *bms_input, uint32_t msTicks) {
    if((msTicks-last_bms_contactor_weld_time) > BMS_CONTACTOR_WELD_PERIOD) {
        can0_ContactorWeld_T msg;
        msg.one = bms_input->contactor_weld_one;
        msg.two = bms_input->contactor_weld_two;
        can0_ContactorWeld_Write(&msg);
        last_bms_contactor_weld_time = bms_input-> msTicks;
    }
}

void can_transmit_pack_status(BMS_INPUT_T *bms_input, uint32_t msTicks) {
    if((msTicks - last_bms_pack_status_time) > BMS_PACK_STATUS_PERIOD) {
        can0_BmsPackStatus_T msg;
        /*

            Fill msg
    
        */

        can0_BmsPackStatus_Write(&msg);
        last_bms_pack_status_time = bms_input->msTicks;
    }
}

void can_transmit_bms_soc(BMS_INPUT_T *bms_input, uint32_t msTicks) {
    if((msTicks - last_bms_soc_time) > BMS_SOC_PERIOD) {
        can0_BMS_SOC_T msg;
        /*

            Fill msg
    
        */

        can0_BMS_SOC_Write(&msg);
        last_bms_soc_time = bms_input->msTicks;
    }
}

void can_transmit_bms_errors(BMS_INPUT_T *bms_input, uint32_t msTicks) {
    if((msTicks - last_bms_errors_time) > BMS_ERRORS_PERIOD) {
        /*

            Fill msg
    
        */

        ERROR_STATUS_T* start_index = Get_Errors();

        int i;
        for(i =0; i<(ERROR_NUM_ERRORS+2); i++) {
            can0_BMSErrors_T msg;
            if((*(start_index+i)).error == true) {
                msg.type = i+1;
                can0_BMSErrors_Write(&msg);
            }
        }

        last_bms_errors_time = bms_input->msTicks;
    }
}