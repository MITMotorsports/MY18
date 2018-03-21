
#include "charge_fault.h"
#include "contactors.h"

void initChargeFault() {
	printf("\r\n[ERROR]: PRECHARGE TOOK TOO LONG TO GET TO DESIRED DC BUS VOLTAGE\r\n");

	openLowSideContactor();
	openHighSideContactor();
}

void loopChargeFault() {
	openLowSideContactor();
	openHighSideContactor();
}
