#ifndef __ERROR_H
#define __ERROR_H

void Error_Handler(const char *s);

typedef uint32_t Time_T;    // in ms
typedef uint16_t Voltage_T; // in dV

// DE for double evaluate - dangerous stuff
#define MAX_DE(a, b) ((a > b) ? a : b)
#define MIN_DE(a, b) ((a < b) ? a : b)

// These are divided separately for a reason.
#define AVG(a, b)    (a / 2 + b / 2)

// #define ONEDGE(init, condition, rising, falling) ( \
//   static last_val = (init);\
//   bool   curr_val = (condition);\
//   if (curr_val != last_val) {\
//     last_val = curr_val;\
//     if (curr_val) {\
//       rising\
//     }\
//     else {\
//       falling\
//     }\
//   }\
// }

#endif // ifndef __ERROR_H
