#include "gb_gps.h"

GbFix GbGps::GetFix() {
  bool fix_done;
  NMEAGPS nmea_gps;
  gps_fix nmea_gps_fix;
  GbFix fix;

  delay(2005); // to trigger staleFix because millis stop during sleep
  GpsOn();

  fix_done = false;
  while (!fix_done) {
    while (nmea_gps.available(*_gps_port)) {
      nmea_gps_fix = nmea_gps.read();
      Serial.print("Fix: ");
      Serial.print(nmea_gps_fix.latitude(), 4);
      fix.latitude = nmea_gps_fix.latitude();
      Serial.print(", ");
      Serial.print(nmea_gps_fix.longitude(), 4);
      fix.longitude = nmea_gps_fix.longitude();
      Serial.print(", ");
      NeoGPS::time_t &dt = nmea_gps_fix.dateTime;
      Serial << dt;
      fix.year = nmea_gps_fix.dateTime.year;
      fix.month = nmea_gps_fix.dateTime.month;
      fix.day = nmea_gps_fix.dateTime.date;
      fix.hour = nmea_gps_fix.dateTime.hours;
      fix.minute = nmea_gps_fix.dateTime.minutes;
      fix.second = nmea_gps_fix.dateTime.seconds;
      Serial.print(", ");
      Serial.print(nmea_gps_fix.satellites);
      Serial.println();
    }

    if (nmea_gps_fix.valid.location && nmea_gps_fix.valid.date &&
        nmea_gps_fix.valid.time && (nmea_gps_fix.satellites > 3)) {
      Serial.println(F("Fix done."));
      fix_done = true;
    }
  }
  
  GpsOff();
  return fix;
}

void GbGps::GpsOn() {
  digitalWrite(_gps_power_pin1, HIGH);
  digitalWrite(_gps_power_pin2, HIGH);
  Serial.println(F("GPS on."));
}

void GbGps::GpsOff() {
  digitalWrite(_gps_power_pin1, LOW);
  digitalWrite(_gps_power_pin2, LOW);
  Serial.println(F("GPS off."));
}
