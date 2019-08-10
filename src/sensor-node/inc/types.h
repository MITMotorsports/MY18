#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "CANlib.h"
typedef struct {
  uint16_t steering_pot;

  uint32_t last_updated;
} ADC_Input_T;

typedef enum {
  LEFT_32,
  RIGHT_32,
  LEFT_16,
  RIGHT_16,
  NUM_WHEELS
} Wheel_T;

typedef struct {
  ADC_Input_T *adc;
  uint32_t msTicks;
} Input_T;

#endif // TYPES_H
