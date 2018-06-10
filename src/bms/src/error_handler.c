#include "error_handler.h"

// TODO: Keep adding things.
static ERROR_STATUS_T error_vector[ERROR_NUM_ERRORS];

//Specifies which errors we care about, by index
static bool errors_to_check[ERROR_NUM_ERRORS];

static ERROR_HANDLER_STATUS_T _Error_Handle_Count(ERROR_STATUS_T *er_stat,
                                                  uint32_t        timeout_num) {
  if (!er_stat->error) {
    er_stat->handling = false;
    return FINE;
  } else {
    // [TODO] magic numbers changeme
    if (er_stat->count < timeout_num) {
      er_stat->handling = true;
      return FINE;
    } else {
      return FAULT;
    }
  }
}

static ERROR_HANDLER_STATUS_T _Error_Handle_Timeout(ERROR_STATUS_T *er_stat,
                                                    uint32_t        timeout_ms) {
  // Board_Println("Call to Handle Timeout");
  if (er_stat->error == false) {
    er_stat->handling = false;
    return FINE;
  } else {
    // [TODO] magic numbers change
    if (msTicks - er_stat->time_stamp < timeout_ms) {
      er_stat->handling = true;
      return FINE;
    } else {
      return FAULT;
    }
  }
}

static ERROR_HANDLER error_handler_vector[ERROR_NUM_ERRORS] = {
  { _Error_Handle_Count,   LTC_PEC_count              },
  { _Error_Handle_Count,   LTC_CVST_count             },
  { _Error_Handle_Count,   LTC_OWT_count              },
  { _Error_Handle_Timeout, L_CONTACTOR_ERROR_time     },
  { _Error_Handle_Timeout, H_CONTACTOR_ERROR_time     },
  { _Error_Handle_Timeout, L_CONTACTOR_WELD_time      },
  { _Error_Handle_Timeout, H_CONTACTOR_WELD_time      },
  { _Error_Handle_Timeout, CELL_UNDER_VOLTAGE_time    },
  { _Error_Handle_Timeout, CELL_OVER_VOLTAGE_time     },
  { _Error_Handle_Timeout, CELL_UNDER_TEMP_time       },
  { _Error_Handle_Timeout, CELL_OVER_TEMP_time        },
  { _Error_Handle_Count,   CONTROL_FLOW_count         },
  { _Error_Handle_Count,   FORCE_OVER_CAN_count       },
};

void Error_Init(void) {
  uint32_t i;

  for (i = 0; i < ERROR_NUM_ERRORS; ++i) {
    error_vector[i].error      = false;
    error_vector[i].handling   = false;
    error_vector[i].time_stamp = 0;
    error_vector[i].count      = 0;

    Error_Recognize(i);
  }

  Error_Ignore(ERROR_H_CONTACTOR_WELD);
  Error_Ignore(ERROR_H_CONTACTOR_ERROR);
}

void Error_Present(ERROR_T er_t) {
  // switch (er_t) {
  // // LTC6804 errors that imply PEC fine should implicitly pass PEC
  // case ERROR_LTC6804_CVST:
  // case ERROR_LTC6804_OWT:
  //   Error_Pass(ERROR_LTC6804_PEC);
  //   break;

  // default:
  //   break;
  // }
  // Board_Println(ERROR_NAMES[er_t]);
  if (!error_vector[er_t].error) {
    error_vector[er_t].error      = true;
    error_vector[er_t].time_stamp = msTicks;
    error_vector[er_t].count      = 1;
  }
  else {
    error_vector[er_t].count += 1;
  }
}

void Error_Clear(ERROR_T er_t) {
  error_vector[er_t].error = false;
}

static ERROR_HANDLER_STATUS_T _Error_Handle_Count_and_Timeout(
  ERROR_STATUS_T *er_stat,
  uint32_t        timeout_ms,
  uint32_t        timeout_num) {
  if (er_stat->error == false) {
    er_stat->handling = false;
    return FINE;
  } else {
    // [TODO] magic numbers changem
    if ((msTicks - er_stat->time_stamp < timeout_ms) &&
        (er_stat->count < timeout_num)) {
      er_stat->handling = true;
      return FINE;
    } else {
      return FAULT;
    }
  }
}

//to be called in loop in main
bool Error_Should_Fault(void) {
  for (ERROR_T i = 0; i < ERROR_NUM_ERRORS; ++i) {
    // if error is present and we care
    if (Check_Error(i, false)) {
      return true;
    }
  }

  return false;
}

//called when checking which errors to send over CAN
bool Check_Error(ERROR_T er_t, bool Force_Check) {
  if (Error_Care(er_t) || Force_Check) {
    return error_handler_vector[er_t].handler(&error_vector[er_t], error_handler_vector[er_t].timeout) == FAULT;
  }
  else {
    return false;
  }
}

const ERROR_STATUS_T* Error_GetStatus(ERROR_T er_t) {
  return &error_vector[er_t];
}

ERROR_STATUS_T* Get_Errors(void) {
  return error_vector;
}

inline bool Error_Care(ERROR_T er_t) {
  return errors_to_check[er_t];
}

inline void Error_Ignore(ERROR_T er_t) {
  errors_to_check[er_t] = false;
}

inline void Error_Recognize(ERROR_T er_t) {
  errors_to_check[er_t] = true;
}
