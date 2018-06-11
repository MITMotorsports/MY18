#include "adc_spi.h"

static const int ext_ch[8] = {0, 8, 16, 24, 32, 40, 48, 56};

static uint8_t Tx_Buf[BUFFER_SIZE];
static uint16_t Rx_Buf[BUFFER_SIZE];

static SSP_ConfigFormat ssp_format;
static Chip_SSP_DATA_SETUP_T xf_setup;
static volatile uint8_t isXferCompleted = 0;


static void Init_SSP_PinMux(void) {
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MISO0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MOSI0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* SSEL0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_10, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SCK0 */
	Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO2_11);
}

static void Buffer_Init(unsigned ch) {
	for (unsigned i = 0; i < BUFFER_SIZE; i++) {
		Tx_Buf[i] = ext_ch[ch];
		Rx_Buf[i] = 0x0;
	}
}

void Init_SPI_ADC(void) {
	Init_SSP_PinMux();
	Chip_SSP_Init(LPC_SSP);
	Chip_SSP_SetBitRate(LPC_SSP, 30000);
	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_SetMaster(LPC_SSP, SSP_MODE_TEST);
	Chip_SSP_Enable(LPC_SSP);
	Buffer_Init(0);
}

void Read_SPI_ADC(unsigned ch) {
	Buffer_Init(ch);
	Chip_SSP_WriteFrames_Blocking(LPC_SSP, Tx_Buf, BUFFER_SIZE);
	Chip_GPIO_SetPinState(LPC_GPIO, CS, true);
	Chip_SSP_ReadFrames_Blocking(LPC_SSP, Rx_Buf, BUFFER_SIZE);
}

void Read_SPI_ADC_Range(uint16_t* ext_adc_data, unsigned start, unsigned stop, unsigned step) {
	for (unsigned ch = start; ch < stop; ch += step) {
		SPI_Read_ADC(ch);
		ext_adc_data[ch] = Rx_Buf[0];
	}
}
