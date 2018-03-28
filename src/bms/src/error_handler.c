#include "error_handler.h"

// TODO: Keep adding things.

static ERROR_HANDLER error_handler_vector[ERROR_NUM_ERRORS] = {
  { _Error_Handle_Count,   LTC_PEC_count                    },
  { _Error_Handle_Count,   LTC_CVST_count                   },
  { _Error_Handle_Count,   LTC_OWT_count                    },
  { _Error_Handle_Timeout, CELL_UNDER_VOLTAGE_time          },
  { _Error_Handle_Timeout, CELL_OVER_VOLTAGE_time           },
  { _Error_Handle_Timeout, CELL_UNDER_TEMP_time             },
  { _Error_Handle_Timeout, CELL_OVER_TEMP_time              },
  { _Error_Handle_Timeout, L_CONTACTOR_WELDED_time          },
  { _Error_Handle_Timeout, H_CONTACTOR_WELDED_time          },
};

void Error_Init(void) {
  uint32_t i;

  for (i = 0; i < ERROR_NUM_ERRORS; ++i) {
    error_vector[i].error      = false;
    error_vector[i].handling   = false;
    error_vector[i].time_stamp = 0;
    error_vector[i].count      = 0;
  }
}

void Error_Assert(ERROR_T er_t) {
  switch (er_t) {
  // LTC6804 errors that imply PEC fine should implicitly pass PEC
  case ERROR_LTC6804_CVST:
  case ERROR_LTC6804_OWT:
    Error_Pass(ERROR_LTC6804_PEC);
    break;

  default:
    break;
  }

  if (!error_vector[er_t].error) {
    error_vector[er_t].error      = true;
    error_vector[er_t].time_stamp = msTicks;
    error_vector[er_t].count      = 1;
  }
  else {
    error_vector[er_t].count += 1;
  }
}

void Error_Pass(ERROR_T er_t) {
  error_vector[er_t].error = false;
}

static ERROR_HANDLER_STATUS_T _Error_Handle_Timeout(ERROR_STATUS_T *er_stat,
                                                    uint32_t        timeout_ms) {
  if (er_stat->error == false) {
    er_stat->handling = false;
    return HANDLER_FINE;
  } else {
    // [TODO] magic numbers change
    if (msTicks - er_stat->time_stamp < timeout_ms) {
      er_stat->handling = true;
      return HANDLER_FINE;
    } else {
      return HANDLER_HALT;
    }
  }
}

static ERROR_HANDLER_STATUS_T _Error_Handle_Count(ERROR_STATUS_T *er_stat,
                                                  uint32_t        timeout_num) {
  if (!er_stat->error) {
    er_stat->handling = false;
    return HANDLER_FINE;
  } else {
    // [TODO] magic numbers changeme
    if (er_stat->count < timeout_num) {
      er_stat->handling = true;
      return HANDLER_FINE;
    } else {
      return HANDLER_HALT;
    }
  }
}

static ERROR_HANDLER_STATUS_T _Error_Handle_Count_and_Timeout(
  ERROR_STATUS_T *er_stat,
  uint32_t        timeout_ms,
  uint32_t        timeout_num) {
  if (er_stat->error == false) {
    er_stat->handling = false;
    return HANDLER_FINE;
  } else {
    // [TODO] magic numbers changem
    if ((msTicks - er_stat->time_stamp < timeout_ms) &&
        (er_stat->count < timeout_num)) {
      er_stat->handling = true;
      return HANDLER_FINE;
    } else {
      return HANDLER_HALT;
    }
  }
}

ERROR_HANDLER_STATUS_T Error_Handle() {
  for (ERROR_T i = 0; i < ERROR_NUM_ERRORS; ++i) {
    if (Error_ShouldHalt(i)) {
      return HANDLER_HALT;
    }
  }
  return HANDLER_FINE;
}

bool Error_ShouldHalt(ERROR_T i) {
  if (error_vector[i].error || error_vector[i].handling) {
    return (error_handler_vector[i].handler(&error_vector[i], msTicks,
                                            error_handler_vector[i].timeout)
            == HANDLER_HALT)
  }
  return false;
}

const ERROR_STATUS_T* Error_GetStatus(ERROR_T er_t) {
  return &error_vector[er_t];
}

ERROR_STATUS_T* Get_Errors(void) {
  return error_vector;
}
