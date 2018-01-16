#include "../inc/fsae_can.h"

#include "../../../lib/CANlib/can_raw.h"

void Fsae_Can_Init(uint32_t baud_rate) {
	Can_Init(baud_rate);
}
void Can_Receive(){

}
