#ifndef __STATE_CSB_H
#define __STATE_CSB_H

#include "state_csb_charge.h"
#include "state_csb_precharge.h"
#include "state_csb_root.h"
#include "pins.h"
#include <stdint.h> //For uint_32t
#include "main.h" //For can_data

typedef enum{
	CSB_STATE_ROOT=0,
	CSB_STATE_PRECHARGE=1,
	CSB_STATE_CHARGE=2
} CSB_STATE_T;


void init_csb_state(void);
//
CSB_STATE_T set_csb_state(CSB_STATE_T newState);
void advance_csb_state(void);
CSB_STATE_T get_csb_state(void);

#endif//#ifndef __STATE_CSB_H
