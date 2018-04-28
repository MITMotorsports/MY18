#include <stdint.h>

#include <FlexCAN.h>
#include <SD.h>
#include <SPI.h>

#ifndef __MK66FX1M0__
  # error "Only a Teensy 3.6 with a dual CAN bus is worthy of being DAQBOI."
#endif // ifndef __MK66FX1M0__

static CAN_message_t msg;

class CommonListener : public CANListener {
public:

  const uint8_t port_num;

  CommonListener(int port_num) : port_num{port_num} {}

  void printFrame(Stream& out, CAN_message_t& frame, int mailbox);

  void gotFrame(CAN_message_t& frame, int mailbox);
};

void CommonListener::printFrame(Stream& out, CAN_message_t& frame, int mailbox) {
  out.print(this->port_num);
  out.write('\t');

  out.print(frame.id, HEX);
  out.write('\t');

  for (size_t c = 0; c < frame.len; c++) {
    out.print(frame.buf[c], HEX);
    out.write(' ');
  }

  out.write('\r');
  out.write('\n');
}

void CommonListener::gotFrame(CAN_message_t& frame, int mailbox) {
  printFrame(Serial, frame, mailbox);

  File log_file = SD.open("datalog.txt", FILE_WRITE);

  if (log_file) {
    printFrame(log_file, frame, mailbox);
    log_file.close();
  }
  else {
    Serial.println("error opening file");
  }
}

CommonListener CANListener0(0);
CommonListener CANListener1(1);

void setup(void) {
  Serial.begin(9600);

  while (!Serial);
  Serial.println("DAQBOI v0.1");

  Can0.begin(500000);
  Can1.begin(500000);

  // Configuring the transceivers
  pinMode(24, OUTPUT);
  pinMode(5,  OUTPUT);

  digitalWrite(24, LOW);
  digitalWrite(5,  LOW);

  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("Card failed, or not present");

    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  // Attach interrupted listeners
  Can0.attachObj(&CANListener0);
  CANListener0.attachGeneralHandler();

  Can1.attachObj(&CANListener1);
  CANListener1.attachGeneralHandler();
}

void loop(void) {}
