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

CommonListener CANListener[] = {CommonListener(0), CommonListener(1)};

void setup(void) {
  Serial.begin(115200);

  #if DEBUG_UART
    while (!Serial);
  #endif

  Serial.println(F("DAQBOI v0.1"));

  /// TELEMETRY
  init_telemetry();

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
  init_logfile();

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


File log_file;

void init_logfile() {
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
}

void save(const LoggedFrame &lf) {
  static size_t saved = 0;

  // If logfile is approaching the limits of FAT32, start a new one
  if (saved >= 4294967296 - 100000) {
    init_logfile();
    saved = 0;
  }

  saved += log_file.println(lf);
}

void print(const LoggedFrame &lf) {
  Serial.println(lf);
}

void loop(void) {
  // http://harmful.cat-v.org/software/c++/linus
  // #if DEBUG_UART
  //   #define PRINT print
  // #else
  //   #define PRINT
  // #endif
  // static Tickerplant<LoggedFrame> tick({save, PRINT});
  //
  // #undef PRINT

  for (auto &listener : CANListener) {
    listener.printIfFull(log_file);
    #if DEBUG_UART
      listener.printIfFull(Serial);
    #endif


    static LoggedFrame lf;
    while (listener.buffer.remove(&lf)) {
      // tick.publish(loggedframe);

      save(lf);
      #if DEBUG_UART
        print(lf);
      #endif
      transmit(lf);
    }
  }

  log_file.flush();

  static bool led_state = false;
  static uint32_t last_flip = millis();

  if (millis() - last_flip > 500) {
    digitalWrite(LED_BUILTIN, led_state);

    // LoggedFrame lf;
    // lf.time = millis();
    // lf.frame.id = 0x1ac;
    // lf.frame.buf[0] = 0xAC;
    // lf.frame.buf[1] = 0xDC;
    // lf.frame.len = 2;
    // transmit(lf);

    led_state = !led_state;
    last_flip = millis();
  }
}
