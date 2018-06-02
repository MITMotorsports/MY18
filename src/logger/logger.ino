#include <stdint.h>

#include <SPI.h>
#include <FlexCAN.h>
#include <SD.h>
#include <TimeLib.h>

#ifndef __MK66FX1M0__
  # error "Only a Teensy 3.6 with a dual CAN bus is worthy of being DAQBOI."
#endif // ifndef __MK66FX1M0__

#define BUILTIN_LED 13

// Macro for converting and padding (to len 2) a number to a String.
#define zfc2(num) ((num < 10)? "0" + String(num) : String(num))

String dir_name;
String log_name;

void set_failover_filename(String& filename) {
  filename = "failover.tsv";
}

class CommonListener : public CANListener {
public:

  const uint8_t port_num;

  CommonListener(uint8_t port_num) : port_num{port_num} {}

  void printFrame(Stream& out, CAN_message_t& frame, int mailbox);

  void gotFrame(CAN_message_t& frame, int mailbox);
};

void CommonListener::printFrame(Stream& out, CAN_message_t& frame, int mailbox) {
  out.print(millis());
  out.write('\t');

  out.print(this->port_num);
  out.write('\t');

  out.print(frame.id, HEX);
  out.write('\t');

  // Length is implicitly defined by number of chars in data field
  // out.print(frame.len, HEX);
  // out.write('\t');

  for (size_t c = 0; c < frame.len; ++c) {
    if (frame.buf[c] < 16) out.write('0');
    out.print(frame.buf[c], HEX);
  }

  out.write('\r');
  out.write('\n');
}

void CommonListener::gotFrame(CAN_message_t& frame, int mailbox) {
  // printFrame(Serial, frame, mailbox);

  File log_file = SD.open(log_name.c_str(), FILE_WRITE);

  if (log_file) {
    printFrame(log_file, frame, mailbox);
    log_file.close();
  }
  else {
    Serial.println("[ERROR] Can't open log_file.");
  }
}

CommonListener CANListener0(0);
CommonListener CANListener1(1);

void setup(void) {
  Serial.begin(9600);

  // while (!Serial);

  Serial.println(F("DAQBOI v0.1"));

  Can0.begin(500000);
  Can1.begin(500000);

  // GPIO

  /// Transceiver Enable
  pinMode(24, OUTPUT);
  pinMode(5,  OUTPUT);

  digitalWrite(24, LOW);
  digitalWrite(5,  LOW);

  /// LED
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  // Set up current time from RTC
  setSyncProvider(getTeensy3Time);
  if (timeStatus()  != timeSet) {
    Serial.println(F("[ERROR] Unable to sync with the RTC."));

    set_failover_filename(log_name);
  }
  else {
    Serial.print(F("RTC has set the system time to "));
    Serial.print(date_string());
    Serial.write(' ');
    Serial.println(time_string());

    // Set log filename to current datetime
    log_name = date_string() + "/" + time_string() + ".tsv";
  }

  while (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println(F("[ERROR] Card failed or not present. Retrying soon..."));
    delay(1000);
  }

  Serial.println(F("card initialized"));

  // Make log directory
  dir_name = date_string();
  if (!SD.mkdir(dir_name.c_str())) {
    Serial.print("[ERROR] failed to make directory with name ");
    Serial.println(dir_name);

    set_failover_filename(log_name);
  }

  // Write datetime string at the beginning of file
  File log_file = SD.open(log_name.c_str(), FILE_WRITE);

  if (log_file) {
    // New log begin delimiter
    log_file.write('~');

    log_file.print(millis());
    log_file.write('\t');

    log_file.print(date_string());
    log_file.write('\t');

    log_file.println(time_string());
  }
  else {
    Serial.println("[ERROR] Can't open log_file.");
  }
  log_file.close();

  // Attach interrupted listeners
  Can0.attachObj(&CANListener0);
  CANListener0.attachGeneralHandler();

  Can1.attachObj(&CANListener1);
  CANListener1.attachGeneralHandler();
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

String date_string() {
  return zfc2(year()) + zfc2(month()) + zfc2(day());
}

String time_string() {
  return zfc2(hour()) + zfc2(minute()) + zfc2(second());
}

void loop(void) {
  static bool led_state = false;
  static uint32_t last_flip = millis();

  if (millis() - last_flip > 1000) {
    digitalWrite(BUILTIN_LED, led_state);

    led_state = !led_state;
    last_flip = millis();
  }
}
