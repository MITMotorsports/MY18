#ifndef __VCU_CAN_LIB_H
#define __VCU_CAN_LIB_H


// CAN IDs (Standard CAN A)
#define FrontCanNodeOutput__MSG_ID 			0x0D0
#define FrontCanNodeWheelSpeed__MSG_ID 		0x0F0
#define BMSCellTemps__MSG_ID 				0x0F1
#define BmsPackStatus__MSG_ID 				0x0F2
#define BMSErrors__MSG_ID 					0x0D4
#define BMS_SOC__MSG_ID 					0x0D5
#define Dash_Request__MSG_ID 				0x0D2
#define BMSState__MSG_ID 					0x0D3
#define VcuToDash__MSG_ID 					0x0F3
#define RearCanNodeWheelSpeed__MSG_ID 		0x0D1
#define BMS_VCU_Switch__MSG_ID 				0x0F4
#define CurrentSensor_Current__MSG_ID 		0x521
#define CurrentSensor_Voltage__MSG_ID 		0x522
#define CurrentSensor_Power__MSG_ID 		0x526
#define MC_Command__MSG_ID 					0x0C0 // From manual
#define BMS_CSB_Switch__MSG_ID 				0x600
#define BMS_Mode__MSG_ID 					0x601

// CAN IDs (Extended CAN B)
// #define ElconCommand__MSG_ID 				0x1806E5F4
// #define ElconHeartbeat__MSG_ID 				0x18FF50E5



#endif
