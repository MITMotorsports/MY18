#ifndef _ERROR_HANDLER_H
#define _ERROR_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#include "board.h"

#define LTC_PEC_count              100
#define LTC_CVST_count             1
#define LTC_OWT_count              10
#define CELL_UNDER_VOLTAGE_time    1000
#define CELL_OVER_VOLTAGE_time     1000
#define CELL_UNDER_TEMP_time       10000
// #define CELL_OVER_TEMP_time        10000
#define CELL_OVER_TEMP_time        1000
#define L_CONTACTOR_ERROR_time     1000
#define H_CONTACTOR_ERROR_time     1000
#define L_CONTACTOR_WELD_time      3000
#define H_CONTACTOR_WELD_time      3000
#define CONTROL_FLOW_count         1
#define FORCE_OVER_CAN_count       100


typedef enum {
  ERROR_LTC_PEC,
  ERROR_LTC_CVST,
  ERROR_LTC_OWT,
  ERROR_L_CONTACTOR_ERROR,
  ERROR_H_CONTACTOR_ERROR,
  ERROR_L_CONTACTOR_WELD,
  ERROR_H_CONTACTOR_WELD,
  ERROR_CELL_UNDER_VOLTAGE,
  ERROR_CELL_OVER_VOLTAGE,
  ERROR_CELL_OVER_TEMP,
  ERROR_CONTROL_FLOW,
  ERROR_FORCE_OVER_CAN,
  ERROR_NUM_ERRORS
} ERROR_T;

static const char *const ERROR_NAMES[ERROR_NUM_ERRORS] = {
  "ERROR_LTC_PEC",
  "ERROR_LTC_CVST",
  "ERROR_LTC_OWT",
  "ERROR_L_CONTACTOR_ERROR",
  "ERROR_H_CONTACTOR_ERROR",
  "ERROR_L_CONTACTOR_WELD",
  "ERROR_H_CONTACTOR_WELD",
  "ERROR_CELL_UNDER_VOLTAGE",
  "ERROR_CELL_OVER_VOLTAGE",
  "ERROR_CELL_OVER_TEMP",
  "ERROR_CONTROL_FLOW",
  "ERROR_FORCE_OVER_CAN",
};

typedef struct {
  bool     handling;
  bool     error;
  uint16_t count;
  uint32_t time_stamp;
} ERROR_STATUS_T;

typedef enum error_handler_status {
    FINE,
    FAULT
} ERROR_HANDLER_STATUS_T;

typedef ERROR_HANDLER_STATUS_T (*ERROR_HANDLER_FUNC)(ERROR_STATUS_T *,
                                                     const uint32_t);
typedef struct {
  ERROR_HANDLER_FUNC handler;
  const uint32_t     timeout;
} ERROR_HANDLER;

void Error_Assert(ERROR_T error);

bool Error_Should_Fault(void);

bool Check_Error(ERROR_T er_t, bool Force_Check);

bool Error_Care(ERROR_T er_t);

#endif // ifndef _ERROR_HANDLER_H
