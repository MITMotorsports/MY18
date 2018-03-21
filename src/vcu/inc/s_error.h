#include "gpio.h"
#include <stdbool.h>

typedef enum {
  NOMINAL = 0,
  START,
  LOOP_OPEN,
} S_ERROR_T;

typedef enum {
  BMS,
  BPD,
  IMD,
  SDN,
  S_ERROR_MONITOR_T_SIZE
} S_ERROR_MONITOR_T;

typedef struct {
  bool latch_errors[S_ERROR_MONITOR_T_SIZE];
} S_ERROR_DATA_T;

extern S_ERROR_T s_error;
extern S_ERROR_DATA_T s_error_data;

// TODO: Move any elsewhere.
bool any(bool *begin, size_t len);


// TODO: Make iterator like params (begin -> end).
#define ARRAY_SIZE(foo) (sizeof(foo) / sizeof(foo[0]))
#define auto_any(foo) (any(foo, ARRAY_SIZE(foo)))
