
#include "contactor_fault.h"
#include "driving.h"
#include "contactors.h"

void initContactorFault() {
	sendMotorOffCmdMsg();
	resetDrivingValues(); // from the driving file
	
	openLowSideContactor();
	openHighSideContactor();

	printf("\r\n[ERROR]: CONTACTORS OPENED AS A RESULT OF A HARDWARE FAULT\r\n");
}

void loopContactorFault() {
	// Because we are paranoid, let's keep doing this forever
	sendMotorOffCmdMsg();

	openLowSideContactor();
	openHighSideContactor();
}
