#include <stdint.h>

#include <SPI.h>
#include <FlexCAN.h>
#include <SD.h>
#include <TimeLib.h>

#include "buffer.h"
#include "listener.h"
#include "tickerplant.h"

#ifndef __MK66FX1M0__
  # error "Only a Teensy 3.6 with a dual CAN bus is worthy of being DAQBOI."
#endif // ifndef __MK66FX1M0__

#define DEBUG_UART false

// Macro for converting and padding (to len 2) a number to a String.
#define zfc2(num) ((num < 10)? "0" + String(num) : String(num))

File log_file;

CommonListener CANListener[] = {CommonListener(0), CommonListener(1)};

void setup(void) {
  Serial.begin(115200);

  #if DEBUG_UART
    while (!Serial);
  #endif

  Serial.println(F("DAQBOI v0.1"));


  /// GPIO
  // Transceiver Enable
  pinMode(24, OUTPUT);
  pinMode(5,  OUTPUT);

  digitalWrite(24, LOW);
  digitalWrite(5,  LOW);

  // LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


  /// RTC
  setSyncProvider(getTeensy3Time);
  if (timeStatus() != timeSet) {
    Serial.println(F("[ERROR] Unable to sync with the RTC."));
  }
  else {
    Serial.print(F("RTC has set the system time to "));
    Serial.print(date_string());
    Serial.write(' ');
    Serial.println(time_string());
  }

  while (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println(F("[ERROR] Card failed or not present. Retrying soon..."));
    delay(1000);
  }

  Serial.println(F("card initialized"));


  /// FILE
  String log_name = "failover.tsv";

  // Make log directory
  String dir_name = date_string();
  if (SD.mkdir(dir_name.c_str())) {
    // Set log filename to current datetime
    log_name = date_string() + "/" + time_string() + ".tsv";
  }
  else {
    Serial.print("[ERROR] failed to make directory with name ");
    Serial.println(dir_name);
  }

  // Open the main log_file
  // Write datetime string at the beginning of file
  log_file = SD.open(log_name.c_str(), FILE_WRITE);

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

  // Attach interrupted listeners
  Can0.attachObj(&CANListener[0]);
  Can1.attachObj(&CANListener[1]);

  for (auto &listener : CANListener) {
    listener.attachGeneralHandler();
  }

  Serial.println(F("listeners initialized"));

  Can0.begin(500000);
  Can1.begin(500000);

  Serial.println(F("buses initialized"));
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


void save(const LoggedFrame &lf) {
  log_file.println(lf);
}

void print(const LoggedFrame &lf) {
  Serial.println(lf);
}

void loop(void) {
  #if DEBUG_UART
    #define PRINT print
  #else
    #define PRINT
  #endif

  static Tickerplant<LoggedFrame> tick({save, PRINT});

  #undef PRINT

  for (auto &listener : CANListener) {
    if (listener.buffer.full()) {
      log_file.print(millis());
      log_file.write(' ');

      log_file.print(listener.port);
      log_file.write(' ');

      log_file.println("FULL");
    }

    static LoggedFrame loggedframe;
    while (listener.buffer.remove(&loggedframe)) {
      tick.publish(loggedframe);
    }
  }

  log_file.flush();

  static bool led_state = false;
  static uint32_t last_flip = millis();

  if (millis() - last_flip > 500) {
    digitalWrite(LED_BUILTIN, led_state);

    led_state = !led_state;
    last_flip = millis();
  }
}
