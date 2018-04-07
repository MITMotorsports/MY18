#ifndef __FAULT_PRECHARGE_H
#define __FAULT_PRECHARGE_H

#include "state.h"


typedef enum {
  precharge_fault_none,
  precharge_fault_timeout,
  precharge_fault_packvoltage,
} PRECHARGE_FAULT_T;


// void raise_precharge_fault(PRECHARGE_FAULT_T cause);



#endif // ifndef __FAULT_PRECHARGE_H
