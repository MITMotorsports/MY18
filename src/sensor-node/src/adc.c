#include "adc.h"

//unused pins so far: 1_5, 2_1, 2_3, 0_10, 0_9, 0_8, 2_2s
static ADC_CLOCK_SETUP_T adc_setup;
static const uint8_t int_ch[] = {ADC_CH2, ADC_CH3, ADC_CH4, ADC_CH5};

bool speed_1;
bool speed_2;

#define STEERING_PIN IOCON_PIO0_11
#define ACCEL_1_PIN IOCON_PIO1_1
#define ACCEL_2_PIN IOCON_PIO1_2
#define BRAKE_1_PIN IOCON_PIO1_3
#define BRAKE_2_PIN IOCON_PIO1_4

// FUNC 2 is ADC for all pins here except for PIO1_4
#define ACCEL_1_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define ACCEL_2_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define BRAKE_1_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define BRAKE_2_PIN_CONFIG (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_ADMODE_EN)
#define STEERING_PIN_CONFIG (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ADMODE_EN)

#define STEERING_CHANNEL ADC_CH0
#define ACCEL_1_CHANNEL ADC_CH2
#define ACCEL_2_CHANNEL ADC_CH3
#define BRAKE_1_CHANNEL ADC_CH4
#define BRAKE_2_CHANNEL ADC_CH5

static void Disable_Channels(void) {
}

void ADC_Init(void) {
	Chip_ADC_Init(LPC_ADC, &adc_setup);

  Chip_IOCON_PinMuxSet(LPC_IOCON, STEERING_PIN, STEERING_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, STEERING_CHANNEL, ENABLE);
  // Accel 1 / AD2
  Chip_IOCON_PinMuxSet(LPC_IOCON, ACCEL_1_PIN, ACCEL_1_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, ACCEL_1_CHANNEL, ENABLE);
  // Accel 2 / AD3
  Chip_IOCON_PinMuxSet(LPC_IOCON, ACCEL_2_PIN, ACCEL_2_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, ACCEL_2_CHANNEL, ENABLE);
  // Brake 1 / AD4
  Chip_IOCON_PinMuxSet(LPC_IOCON, BRAKE_1_PIN, BRAKE_1_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, BRAKE_1_CHANNEL, ENABLE);
  // Brake 2 / AD5
  Chip_IOCON_PinMuxSet(LPC_IOCON, BRAKE_2_PIN, BRAKE_2_PIN_CONFIG);
  Chip_ADC_EnableChannel(LPC_ADC, BRAKE_2_CHANNEL, ENABLE);

  Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);

	Disable_Channels();

}

uint16_t Read_ADC(ADC_CHANNEL_T channel) {
  uint16_t result = 0;
  Chip_ADC_ReadValue(LPC_ADC, channel, &result);
  return result;
}


void Internal_Read_ADC(uint16_t* int_adc_data) {
	int i;
	for(i = 0; i < sizeof(int_adc_data)/sizeof(int_adc_data[0]); i++) {
		int_adc_data[i] = Read_ADC(int_ch[i]);
	}
}
