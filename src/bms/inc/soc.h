#ifndef _SOC_H_
#include "pack.h"

# define _SOC_H_
# define SOC_EEPROM_INDEX 0
# define SOC_WRITE_PERIOD 20000

static uint32_t init_soc, count;

void SOC_Init(BMS_PACK_STATUS_T *ps);
void SOC_Estimate(BMS_PACK_STATUS_T *ps);
void SOC_Write(BMS_PACK_STATUS_T *ps);
void SOC_Max(BMS_PACK_STATUS_T *ps);

#endif // ifndef _SOC_H_
