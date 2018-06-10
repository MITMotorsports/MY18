#ifndef _CONFIG_H_
#define _CONFIG_H_

/**
 * TODO this file really should not need to exist.
 * Is there a way to only include certain files from evt_lib
 * so that the project does not need to implicitly include
 * the ltc6804 drivers? ltc6804.c has a constraint expecting this
 * file with these defines, which doesn't make sense for this project.
 */

#define MAX_NUM_MODULES 15
#define MAX_CELLS_PER_MODULE 12

#define LTC6804_BAUD 600000
#define CAN_BAUD 500000
#define EEPROM_BAUD 600000

#endif
