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

void transmit(const LoggedFrame &lf) {
  switch (lf.frame.id) {
    case 0x0F2:  // CellVoltageRange
    case 0x0F1:  // CellTemperatureRange
    case 0x0F0:  // CellTemperatureVariance
    case 0x00C:  // Test
      break;
    default:
      return;
  }

  LIMIT(200);

  Serial.print("[XBEE TX] ");
  Serial.println(lf);

  uint8_t data[] = {
    lf.frame.id,
    lf.frame.len,
    lf.frame.buf[0],
    lf.frame.buf[1],
    lf.frame.buf[2],
    lf.frame.buf[3],
    lf.frame.buf[4],
    lf.frame.buf[5],
    lf.frame.buf[6],
    lf.frame.buf[7],
  };

  Tx16Request tx = Tx16Request(0xFFFF, data, sizeof(data));

  xbee.send(tx);
}
