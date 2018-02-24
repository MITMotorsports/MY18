#ifndef _ERROR_HANDLER_H
#define _ERROR_HANDLER_H

#include <stdint.h>
#include <stdbool.h>


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

static const char * const ERROR_NAMES[ERROR_NUM_ERRORS] = {
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


static const char * const ERROR__HB_NAMES[ERROR_NUM_ERRORS] = {
    "HBEAT_DI",
    "HBEAT_MI"
};


typedef enum error_handler_status {
    HANDLER_FINE,
    HANDLER_HALT
} ERROR_HANDLER_STATUS_T;

typedef struct error_status {
    bool        handling;
    bool        error;
    uint16_t    count;
    uint32_t    time_stamp;
} ERROR_STATUS_T;

typedef  ERROR_HANDLER_STATUS_T (*ERROR_HANDLER_FUNC)(ERROR_STATUS_T* , const uint32_t, const uint32_t);

typedef struct ERROR_HANDLER {
	ERROR_HANDLER_FUNC handler;
	const uint32_t timeout;
} ERROR_HANDLER;



void Error_Init(void);
void Error_Assert(ERROR_T er_t, uint32_t msTicks);
void Error_Pass(ERROR_T er_t);
void Error_HB(HBEAT_T hb);

const ERROR_STATUS_T *  Error_GetStatus(ERROR_T er_t);
bool Error_ShouldHalt(ERROR_T er_t, uint32_t msTicks);
ERROR_HANDLER_STATUS_T Error_Handle(uint32_t msTicks);

const ERROR_STATUS_T * Error_HB_GetStatus(HBEAT_T hb);
ERROR_HANDLER_STATUS_T Error_HB_Handle(uint32_t msTicks);

#endif
