#ifndef DISCHARGE_H
#define DISCHARGE_H

#include "state_types.h"
#include "pack.h"

void Discharge_Init(BMS_STATE_T *state);
void Discharge_Config(BMS_PACK_CONFIG_T *pack_config);
void Discharge_Step(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output);
void Discharge_Step(BMS_INPUT_T *input, BMS_STATE_T *state, BMS_OUTPUT_T *output);
uint32_t Read_Max_Current(void);

#endif
