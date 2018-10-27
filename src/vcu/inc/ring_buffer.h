
#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include <stdint.h>

#ifndef ind_min
# define ind_min(a,b) ( ((a) < (b)) ? (a) : (b) )
#endif

typedef struct {
  int32_t* storage;
  int32_t* end;
  uint16_t size;
  volatile int32_t* read;
  volatile int32_t* write;
  volatile uint16_t available;
} RingBuffer32;

void RingBuffer32_init(RingBuffer32* ring, int32_t* storage, uint16_t size);
uint16_t RingBuffer32_available(RingBuffer32* ring);
uint16_t RingBuffer32_free(RingBuffer32* ring);
void RingBuffer32_clear(RingBuffer32* ring);
void RingBuffer32_read(RingBuffer32* ring, int32_t* buffer, uint16_t size);
int32_t RingBuffer32_readByte(RingBuffer32* ring);
void RingBuffer32_write(RingBuffer32* ring, const int32_t* buffer, uint16_t size);
void RingBuffer32_writeByte(RingBuffer32* ring, int32_t b);
uint16_t RingBuffer32_readUntil(RingBuffer32* ring, char* buffer, uint16_t size, int32_t stopByte);
uint16_t RingBuffer32_readLine(RingBuffer32* ring, char* buffer, uint16_t size);
int32_t RingBuffer32_peek(RingBuffer32* ring);
int32_t RingBuffer32_peekn(RingBuffer32* ring, uint16_t i);

#endif