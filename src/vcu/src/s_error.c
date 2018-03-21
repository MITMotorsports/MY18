#include "s_error.h"

S_ERROR_T s_error;
S_ERROR_T previous_s_error;
S_ERROR_DATA_T s_error_data;

inline bool any(bool *begin, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (begin[i]) return true;
  }

  return false;
}

void s_error_setup() {
  // printf("\r\nERROR ROOT\r\n");

  s_error = previous_s_error = START;

  // Assume all errors are clear on boot. No security concern, we'll find out in
  // a few ms if it's true.
  memset(s_error_data.latch_errors, false, S_ERROR_MONITOR_T_SIZE);
}

void s_error_enter() {}

void s_error_loop() {
  // The following pins are normally set. Invert to see if error.
  s_error_data.latch_errors[BMS] = ! READ_PIN(BMS_GATE);
  s_error_data.latch_errors[BPD] = ! READ_PIN(BPD_GATE);
  s_error_data.latch_errors[IMD] = ! READ_PIN(IMD_GATE);
  s_error_data.latch_errors[SDN] = ! READ_PIN(SDN_GATE);

  bool any_latch_errors = auto_any(s_error_data.latch_errors);

  S_ERROR_T next_s_error;

  // switch (s_error) {
  // case START:
  //
  //   // Monitor latch errors, but ignore anything else that may open shutdown
  //   // loop. This will hapen until we close any contactor.
  //
  //   if (any_latch_errors) next_s_error = L
  //                                        break;
  //
  // case NOMINAL:
  //
  //   // Monitor all vital errors.
  //   break;
  //
  // // from now on: case <unresetable error>:
  // case LOOP_OPEN:
  //   next_s_error = s_error;
  //   break;
  // }

  previous_s_error = s_error;

  // s_error          = next_s_error;
}

void s_error_exit() {}
