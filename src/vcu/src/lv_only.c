
#include "lv_only.h"

void initLVOnly() {
	printf("\r\nCAR STARTED IN LV MODEr\n");
}

void loopLVOnly() {
	if (buttons.driverResetPressed) {
		changeCarMode(CAR_STATE_PRECHARGING);
	}
}
