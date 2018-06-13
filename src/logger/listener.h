#pragma once
#include "buffer.h"

// #define LOG_BUFFER_SIZE 256

typedef struct {
  uint32_t time;
  uint8_t port;
  CAN_message_t frame;
} LoggedFrame;

class CommonListener : public CANListener {
public:
  const uint8_t port;
  FIFOCircBuffer<LoggedFrame> buffer;

  CommonListener(uint8_t port) : port(port), buffer(256) {}
  void gotFrame(CAN_message_t &frame, int mailbox);
};

void CommonListener::gotFrame(CAN_message_t &frame, int mailbox) {
  LoggedFrame loggedframe = {};

  loggedframe.time = millis();
  loggedframe.port = port;
  loggedframe.frame = frame;

  buffer.add(loggedframe);
}
