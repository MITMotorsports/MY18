#ifndef _ERROR_HANDLER_H
#define _ERROR_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#define CELL_OVER_VOLTAGE_timeout_ms    1000
#define CELL_UNDER_VOLTAGE_timeout_ms   1000
#define OVER_CURRENT_timeout_ms                 500
#define LTC6802_PEC_timeout_count               100
#define LTC6802_CVST_timeout_count              2
#define LTC6802_OWT_timeout_count               10
#define CAN_timeout_count                               5
#define EEPROM_timeout_count                    5
#define CONFLICTING_MODE_REQUESTS_count 2
#define BLOWN_FUSE_timeout_count        1
#define CONTACTOR_WELDED_timeout_count  1


#define CELL_OVER_TEMP_timeout_ms     10000
#define CELL_UNDER_TEMP_timeout_ms    10000
#define VCU_DEAD_count                1
#define CONTROL_FLOW_count            1


typedef enum error {
  ERROR_LTC6804_PEC,
  ERROR_LTC6804_CVST,
  ERROR_LTC6804_OWT,
  ERROR_EEPROM,
  ERROR_CELL_UNDER_VOLTAGE,
  ERROR_CELL_OVER_VOLTAGE,
  ERROR_CELL_UNDER_TEMP,
  ERROR_CELL_OVER_TEMP,
  ERROR_OVER_CURRENT,
  ERROR_CAN,
  ERROR_CONFLICTING_MODE_REQUESTS,
  ERROR_VCU_DEAD,
  ERROR_CONTROL_FLOW,
  ERROR_BLOWN_FUSE,
  ERROR_CONTACTOR_WELDED,
  ERROR_NUM_ERRORS
} ERROR_T;


static const char *const ERROR_NAMES[ERROR_NUM_ERRORS] = {
  "ERROR_LTC6804_PEC",
  "ERROR_LTC6804_CVST",
  "ERROR_LTC6804_OWT",
  "ERROR_EEPROM",
  "ERROR_CELL_UNDER_VOLTAGE",
  "ERROR_CELL_OVER_VOLTAGE",
  "ERROR_CELL_UNDER_TEMP",
  "ERROR_CELL_OVER_TEMP",
  "ERROR_OVER_CURRENT",
  "ERROR_CAN",
  "ERROR_CONFLICTING_MODE_REQUESTS",
  "ERROR_VCU_DEAD",
  "ERROR_CONTROL_FLOW",
  "ERROR_BLOWN_FUSE",
  "ERROR_CONTACTOR_WELDED"
};


typedef enum hbeats {
  HBEAT_DI = (int)ERROR_NUM_ERRORS,
  HBEAT_MI
} HBEAT_T;


static const char *const ERROR__HB_NAMES[ERROR_NUM_ERRORS] = {
  "HBEAT_DI",
  "HBEAT_MI"
};


typedef enum error_handler_status {
  HANDLER_FINE,
  HANDLER_HALT
} ERROR_HANDLER_STATUS_T;


typedef struct error_status {
  bool     handling;
  bool     error;
  uint16_t count;
  uint32_t time_stamp;
} ERROR_STATUS_T;


typedef  ERROR_HANDLER_STATUS_T (*ERROR_HANDLER_FUNC)(ERROR_STATUS_T *,
                                                      const uint32_t,
                                                      const uint32_t);


typedef struct ERROR_HANDLER {
  ERROR_HANDLER_FUNC handler;
  const uint32_t     timeout;
} ERROR_HANDLER;

static ERROR_STATUS_T error_vector[ERROR_NUM_ERRORS];

// Functions
void                          Error_Init(void);

void                          Error_Assert(ERROR_T  er_t);

void                          Error_Pass(ERROR_T er_t);

void                          Error_HB(HBEAT_T hb);

const ERROR_STATUS_T        * Error_GetStatus(ERROR_T er_t);

bool                          Error_ShouldHalt(ERROR_T  er_t);

ERROR_HANDLER_STATUS_T        Error_Handle();

const ERROR_STATUS_T        * Error_HB_GetStatus(HBEAT_T hb);

ERROR_HANDLER_STATUS_T        Error_HB_Handle();

static ERROR_HANDLER_STATUS_T _Error_Handle_Timeout(ERROR_STATUS_T *er_stat,
                                                    uint32_t        timeout_ms);

static ERROR_HANDLER_STATUS_T _Error_Handle_Count(ERROR_STATUS_T *er_stat,
                                                  uint32_t        timeout_num);

static ERROR_HANDLER_STATUS_T _Error_Handle_Count_and_Timeout(
  ERROR_STATUS_T *er_stat,
  uint32_t        timeout_ms,
  uint32_t        timeout_num);


// Data with function ptrs

const static ERROR_HANDLER error_handler_vector[ERROR_NUM_ERRORS] = {
  { _Error_Handle_Count,   LTC6802_PEC_timeout_count                   },
  { _Error_Handle_Count,   LTC6802_CVST_timeout_count                  },
  { _Error_Handle_Count,   LTC6802_OWT_timeout_count                   },
  { _Error_Handle_Count,   EEPROM_timeout_count                        },
  { _Error_Handle_Timeout, CELL_UNDER_VOLTAGE_timeout_ms               },
  { _Error_Handle_Timeout, CELL_OVER_VOLTAGE_timeout_ms                },
  { _Error_Handle_Timeout, CELL_UNDER_TEMP_timeout_ms                  },
  { _Error_Handle_Timeout, CELL_OVER_TEMP_timeout_ms                   },
  { _Error_Handle_Timeout, OVER_CURRENT_timeout_ms                     },
  { _Error_Handle_Count,   CAN_timeout_count                           },
  { _Error_Handle_Count,   CONFLICTING_MODE_REQUESTS_count             },
  { _Error_Handle_Count,   VCU_DEAD_count                              },
  { _Error_Handle_Count,   CONTROL_FLOW_count                          },
  { _Error_Handle_Count,   BLOWN_FUSE_timeout_count                    },
  { _Error_Handle_Count,   CONTACTOR_WELDED_timeout_count              }
};

#endif // ifndef _ERROR_HANDLER_H
