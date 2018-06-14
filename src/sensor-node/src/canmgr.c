#include "canmgr.h"

//#define DEBUG_PRINT true

void can_transmit(uint16_t* ext_adc_data, uint16_t* int_adc_data, Speed_Input_T* speed_val) {
	// can_transmit_adc_data1(ext_adc_data);
	// can_transmit_adc_data2(ext_adc_data);
	can_transmit_adc_data3(int_adc_data);
  can_transmit_wheel_speed(speed_val);
}

// void can_transmit_adc_data1(uint16_t* ext_adc_data) {
// 	LIMIT(can0_ADC_Data1_period);
//
// 	can0_ADC_Data1_T msg;
// 	msg.shockpot = ext_adc_data[SHOCKPOT];
// 	msg.load_cell = ext_adc_data[LOAD_CELL];
// 	msg.ext_adc_ch2 = ext_adc_data[2];
// 	msg.ext_adc_ch3 = ext_adc_data[3];
//  	handle_can_error(can0_ADC_Data1_Write(&msg));
// }

// void can_transmit_adc_data2(uint16_t* ext_adc_data) {
// 	LIMIT(can0_ADC_Data2_period);
//
// 	can0_ADC_Data2_T msg;
// 	msg.ext_adc_ch4 = ext_adc_data[4];
// 	msg.ext_adc_ch5 = ext_adc_data[5];
// 	msg.ext_adc_ch6 = ext_adc_data[6];
// 	msg.ext_adc_ch7 = ext_adc_data[7];
//  	handle_can_error(can0_ADC_Data2_Write(&msg));
// }

void can_transmit_adc_data3(uint16_t* int_adc_data) {
	LIMIT(can0_ADC_Data3_period);

	can0_ADC_Data3_T msg;
	msg.int_adc_ch2 = int_adc_data[0];
	msg.int_adc_ch3 = int_adc_data[1];
	msg.int_adc_ch4 = int_adc_data[2];
	msg.int_adc_ch5 = int_adc_data[3];
 	handle_can_error(can0_ADC_Data3_Write(&msg));
}

void can_transmit_wheel_speed(Speed_Input_T* speed_val) {
  LIMIT(can0_SensorNodeWheelSpeed_period);
    can0_SensorNodeWheelSpeed_T msg;
    msg.right_32b = speed_val->can_node_right_32b_wheel_speed;
    msg.left_32b = speed_val->can_node_left_32b_wheel_speed;
    handle_can_error(can0_SensorNodeWheelSpeed_Write(&msg));

#ifdef DEBUG_PRINT
    Board_Print_BLOCKING("RWS: ");
    Board_PrintNum(msg_right.right_32b, 10);
    Board_Print_BLOCKING("LWS: ");
    Board_PrintNum(msg_left.left_32b, 10);
#endif
}

void handle_can_error(Can_ErrorID_T error){
  if ((error != Can_Error_NONE) && (error != Can_Error_NO_RX)) {
    switch (error) {
    case Can_Error_NONE:
      Board_Print_BLOCKING("Can_Error_NONE\n");
      break;

    case Can_Error_NO_RX:
      Board_Print_BLOCKING("Can_Error_NO_RX\n");
      break;

    case Can_Error_EPASS:
      Board_Print_BLOCKING("Can_Error_EPASS\n");
      break;

    case Can_Error_WARN:
      Board_Print_BLOCKING("Can_Error_WARN\n");
      break;

    case Can_Error_BOFF:
      Board_Print_BLOCKING("Can_Error_BOFF\n");
      break;

    case Can_Error_STUF:
      Board_Print_BLOCKING("Can_Error_STUF\n");
      break;

    case Can_Error_FORM:
      Board_Print_BLOCKING("Can_Error_FORM\n");
      break;

    case Can_Error_ACK:
      Board_Print_BLOCKING("Can_Error_ACK\n");
      break;

    case Can_Error_BIT1:
      Board_Print_BLOCKING("Can_Error_BIT1\n");
      break;

    case Can_Error_BIT0:
      Board_Print_BLOCKING("Can_Error_BIT0\n");
      break;

    case Can_Error_CRC:
      Board_Print_BLOCKING("Can_Error_CRC\n");
      break;

    case Can_Error_UNUSED:
      Board_Print_BLOCKING("Can_Error_UNUSED\n");
      break;

    case Can_Error_UNRECOGNIZED_MSGOBJ:
      Board_Print_BLOCKING("Can_Error_UNRECOGNIZED_MSGOBJ\n");
      break;

    case Can_Error_UNRECOGNIZED_ERROR:
      Board_Print_BLOCKING("Can_Error_UNRECOGNIZED_ERROR\n");
      break;

    case Can_Error_TX_BUFFER_FULL:
      Board_Print_BLOCKING("Can_Error_TX_BUFFER_FULL\n");
      CAN_Flush_Tx();
      CAN_Clear_Error();
      CAN_ResetPeripheral();
      init_can0_sensor_node();
      break;

    case Can_Error_RX_BUFFER_FULL:
      Board_Print_BLOCKING("Can_Error_RX_BUFFER_FULL\n");
      CAN_Flush_Tx();
      CAN_Clear_Error();
      CAN_ResetPeripheral();
      init_can0_sensor_node();
      break;
    }
  }
}
