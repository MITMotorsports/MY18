#pragma once
#include "buffer.h"

#define LOG_BUFFER_SIZE (1 << 11)

class LoggedFrame: public Printable {
public:
  uint32_t time;
  uint8_t  port;
  CAN_message_t frame;

  size_t printTo(Print &p) const;
};

inline size_t LoggedFrame::printTo(Print &p) const {
  size_t s = 0;

  s += p.print(time);
  s += p.write('\t');

  s += p.print(port);
  s += p.write('\t');

  s += p.print(frame.id, HEX);
  s += p.write('\t');

  // Length is implicitly defined by number of chars in data field
  // s += p.print(frame.len, HEX);
  // s += p.write('\t');

  for (size_t c = 0; c < frame.len; ++c) {
    if (frame.buf[c] < 16) s += p.write('0');
    s += p.print(frame.buf[c], HEX);
  }

  return s;
}

static inline void callback(LoggedFrame &loggedframe) {
  if (loggedframe.port == 0) {
    switch (loggedframe.frame.id) {
      case 0x137:  // SBG_EKF_Velocity
      case 0x138:  // SBG_EKF_Velocity_ACC
        Can1.write(loggedframe.frame);
    }
  }
}

class CommonListener: public CANListener {
public:
  uint8_t const port;
  FIFOCircBuffer<LoggedFrame, LOG_BUFFER_SIZE> buffer;

  CommonListener(uint8_t port): port(port) {}
  void gotFrame(CAN_message_t &frame, int mailbox);
  void printIfFull(Print &p);
};

void CommonListener::gotFrame(CAN_message_t &frame, int mailbox) {
  LoggedFrame loggedframe;

  loggedframe.time  = millis();
  loggedframe.port  = port;
  loggedframe.frame = frame;

  buffer.add(loggedframe);
  callback(loggedframe);
}

inline void CommonListener::printIfFull(Print &p) {
  if (buffer.full()) {
    p.print(millis());
    p.write('\t');

    p.print(port);
    p.write('\t');
    p.println("FULL");
  }
}
