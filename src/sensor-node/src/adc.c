#include "adc.h"

//unused pins so far: 1_5, 2_1, 2_3, 0_10, 0_9, 0_8, 2_2s
static ADC_CLOCK_SETUP_T adc_setup;
static const uint8_t int_ch[] = {ADC_CH2, ADC_CH3, ADC_CH4, ADC_CH5};

bool speed_1;
bool speed_2;


static void Disable_Channels(void) {
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, DISABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH3, DISABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH4, DISABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH5, DISABLE);	
}

void ADC_Init(void) {
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_2, IOCON_FUNC2 | IOCON_ADMODE_EN); //ADC 3
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_3, IOCON_FUNC2 | IOCON_ADMODE_EN); //ADC 4
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_4, IOCON_FUNC1 | IOCON_ADMODE_EN); //ADC 5
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, IOCON_FUNC2 | IOCON_ADMODE_EN); // ADC 2

	Chip_ADC_Init(LPC_ADC, &adc_setup);

	Disable_Channels();

}

static uint16_t Read_ADC(int ch) {
	uint16_t adc_data;
	Disable_Channels();
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	Chip_ADC_EnableChannel(LPC_ADC, ch, ENABLE);
	while (!Chip_ADC_ReadStatus(LPC_ADC, ch, ADC_DR_DONE_STAT)) {}

	Chip_ADC_ReadValue(LPC_ADC, ch, &adc_data);
	return adc_data;
	
}


void Internal_Read_ADC(uint16_t* int_adc_data) {
	int i;
	for(i = 0; i < sizeof(int_adc_data)/sizeof(int_adc_data[0]); i++) {
		int_adc_data[i] = Read_ADC(int_ch[i]);
	}
}