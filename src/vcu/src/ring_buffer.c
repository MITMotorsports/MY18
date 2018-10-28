
#include "ring_buffer.h"

void RingBuffer32_init(RingBuffer32* ring, int32_t* storage, uint16_t size) {
  ring->storage = storage;
  ring->size = size;
  ring->end = ring->storage + ring->size;
  ring->read = ring->storage;
  ring->write = ring->storage;
  ring->available = 0;
}

uint16_t RingBuffer32_available(RingBuffer32* ring) {
  return ring->available;
}

uint16_t RingBuffer32_free(RingBuffer32* ring) {
  return ring->size - ring->available;
}

void RingBuffer32_clear(RingBuffer32* ring) {
  ring->read = ring->storage;
  ring->write = ring->storage;
  ring->available = 0;
}

int32_t RingBuffer32_readByte(RingBuffer32* ring) {
  if (ring->available == 0) {
    return 0;
  }
  int32_t ret = *ring->read++;
  ring->available--;
  if (ring->read >= ring->end) {
    ring->read = ring->storage;
  }
  return ret;
}

void RingBuffer32_read(RingBuffer32* ring, int32_t* buffer, uint16_t size) {
  int32_t i;

  // TODO can be optimized
  for (i = 0; i < size; i++) {
    buffer[i] = RingBuffer32_readByte(ring);
  }
}

void RingBuffer32_writeByte(RingBuffer32* ring, int32_t b) {
  if (ring->available >= ring->size) {
    RingBuffer32_readByte(ring);
  }

  *ring->write = b;
  ring->write++;
  ring->available++;
  if (ring->write >= ring->end) {
    ring->write = ring->storage;
  }
}

void RingBuffer32_write(RingBuffer32* ring, const int32_t* buffer, uint16_t size) {
  uint16_t i;

  // TODO can be optimized
  for (i = 0; i < size; i++) {
    RingBuffer32_writeByte(ring, buffer[i]);
  }
}

uint16_t RingBuffer32_readLine(RingBuffer32* ring, char* buffer, uint16_t size) {
  return RingBuffer32_readUntil(ring, buffer, size, '\r');
}

uint16_t RingBuffer32_readUntil(RingBuffer32* ring, char* buffer, uint16_t size, int32_t stopByte) {
  int32_t b;
  uint16_t i;
  for (i = 0; i < ind_min(ring->available, size - 1); i++) {
    b = RingBuffer32_peekn(ring, i);
    if (b == stopByte) {
      i++;
      RingBuffer32_read(ring, (int32_t*) buffer, i);
      buffer[i] = '\0';
      return i;
    }
  }
  buffer[0] = '\0';
  return 0;
}

int32_t RingBuffer32_peek(RingBuffer32* ring) {
  return RingBuffer32_peekn(ring, 0);
}

int32_t RingBuffer32_peekn(RingBuffer32* ring, uint16_t i) {
  if (i >= ring->available) {
    return 0;
  }

  int32_t* read = (int32_t*)ring->read;
  int32_t* p = read + i;
  if (p >= ring->end) {
    p -= ring->size;
  }
  return *p;
}