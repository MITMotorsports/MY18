#include <XBee.h>

#include "buffer.h"
#include "listener.h"

#define XBeeSerial Serial2
static XBee xbee = XBee();

static FIFOCircBuffer<LoggedFrame, 128> tx_buf;

void init_telemetry() {
  XBeeSerial.begin(115200);
  xbee.setSerial(XBeeSerial);
}

#define ONES(len) \
  ((1ULL << (len)) - 1)

#define START_IDX(start, len) \
  (64 - (start) - (len))

#define ZEROES_MASK(start, len) \
  (~(ONES(len) << START_IDX(start, len)))

#define INPUT_MASK(input, start, len) \
  (((input) & ONES(len)) << START_IDX(start, len))

#define INSERT(input, output, start, len) \
  (((output) & (ZEROES_MASK(start, len))) | INPUT_MASK(input, start, len))

#define LIMIT(period)                                       \
  static uint32_t last_sent = 0;                            \
  if (millis() - last_sent < period) return;                \
  last_sent = millis();

void send(const LoggedFrame &lf) {
  LIMIT(400);

  Tx16Request tx = Tx16Request(0xFFFF, (uint8_t*) &lf, sizeof(lf));

  xbee.send(tx);
}
