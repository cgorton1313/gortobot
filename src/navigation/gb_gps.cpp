#include "gb_gps.h"

GbGps::GbGps(uint8_t power_pin1, uint8_t power_pin2, HardwareSerial &port,
             uint32_t baud)
    : GbAbstractGps(), _gps_power_pin1(power_pin1), _gps_power_pin2(power_pin2),
      _gps_port(&port) {
  port.begin(baud);
  pinMode(power_pin1, OUTPUT);
  pinMode(power_pin2, OUTPUT);
  digitalWrite(power_pin1, LOW); // turn off GPS
  digitalWrite(power_pin2, LOW); // turn off GPS
};

GbFix GbGps::GetFix() {
  bool fix_done;
  NMEAGPS nmea_gps;
  gps_fix nmea_gps_fix;
  GbFix fix;
  bool ledOn;

  delay(2005); // to trigger staleFix because millis stop during sleep
  GpsOn();

  fix_done = false;
  while (!fix_done) {
    while (nmea_gps.available(*_gps_port)) {
      nmea_gps_fix = nmea_gps.read();
      DEBUG_PRINT("Fix: ");
      DEBUG_PRINT_XY(nmea_gps_fix.latitude(), 4);
      fix.latitude = nmea_gps_fix.latitude();
      DEBUG_PRINT(", ");
      DEBUG_PRINT_XY(nmea_gps_fix.longitude(), 4);
      fix.longitude = nmea_gps_fix.longitude();
      DEBUG_PRINT(", ");
      NeoGPS::time_t &dt = nmea_gps_fix.dateTime;
      Serial << dt;
      fix.year = nmea_gps_fix.dateTime.year;
      fix.month = nmea_gps_fix.dateTime.month;
      fix.day = nmea_gps_fix.dateTime.date;
      fix.hour = nmea_gps_fix.dateTime.hours;
      fix.minute = nmea_gps_fix.dateTime.minutes;
      fix.second = nmea_gps_fix.dateTime.seconds;
      DEBUG_PRINT(", ");
      DEBUG_PRINT(nmea_gps_fix.satellites);
      DEBUG_PRINTLN();
    }

    if (millis() / 50 % 20 == 0) {
      ledOn = true;
    } else {
      ledOn = false;
    }
    digitalWrite(LED_BUILTIN, ledOn);

    if (nmea_gps_fix.valid.location && nmea_gps_fix.valid.date &&
        nmea_gps_fix.valid.time && (nmea_gps_fix.satellites > 2)) {
      DEBUG_PRINTLN(F("Fix done."));
      fix_done = true;
    }
  }

  digitalWrite(LED_BUILTIN, LOW);
  GpsOff();
  return fix;
}

void GbGps::GpsOn() {
  digitalWrite(_gps_power_pin1, HIGH);
  digitalWrite(_gps_power_pin2, HIGH);
  DEBUG_PRINTLN(F("GPS on."));
}

void GbGps::GpsOff() {
  digitalWrite(_gps_power_pin1, LOW);
  digitalWrite(_gps_power_pin2, LOW);
  DEBUG_PRINTLN(F("GPS off."));
}
