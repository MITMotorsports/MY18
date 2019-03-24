#include <string.h>
#include "eeprom_config.h"
#include "chip.h"
#include "ltc6804.h"
#include "board.h"
#include "pack.h"
#include "state_types.h"
#include "config.h"
#include "console.h"
#include "error_handler.h"
#include "soc.h"

#define EEPROM_CS_PIN 2, 9
#define PRE_ERROR_CHECK_TIMEOUT 500

extern volatile uint32_t msTicks;
