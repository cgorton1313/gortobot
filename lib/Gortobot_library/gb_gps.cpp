#include "gb_gps.h"

GbGps::GbGps(byte power_pin1, byte power_pin2, HardwareSerial &port, unsigned long baud) {
        _gps_power_pin1 = power_pin1;
        _gps_power_pin2 = power_pin2;
        _gps_port = &port;
        port.begin(baud); // fix it later
}

GbFix GbGps::GetFix(char fix_type) {
        bool fix_done;
        NMEAGPS nmea_gps;
        gps_fix nmea_gps_fix;
        GbFix the_fix;
        switch (fix_type) {
        case 'r':
                delay(2005); // to trigger staleFix because millis stop during sleep
                GpsOn();
                fix_done = false;
                while (!fix_done)
                {
                    while (nmea_gps.available(*_gps_port)) // see if we can pass this in
                    {
                        nmea_gps_fix = nmea_gps.read();
                        Serial.print("Fix: ");
                        Serial.print(nmea_gps_fix.latitude(), 4);
                        the_fix.latitude = nmea_gps_fix.latitude();
                        Serial.print(", ");
                        Serial.print(nmea_gps_fix.longitude(), 4);
                        the_fix.longitude = nmea_gps_fix.longitude();
                        Serial.print(", ");
                        NeoGPS::time_t & dt = nmea_gps_fix.dateTime;
                        Serial << dt;
                        the_fix.year = nmea_gps_fix.dateTime.year;
                        the_fix.month = nmea_gps_fix.dateTime.month;
                        the_fix.day = nmea_gps_fix.dateTime.date;
                        the_fix.hour = nmea_gps_fix.dateTime.hours;
                        the_fix.minute = nmea_gps_fix.dateTime.minutes;
                        the_fix.second = nmea_gps_fix.dateTime.seconds;
                        Serial.print(", ");
                        Serial.print(nmea_gps_fix.satellites);
                        Serial.println();
                    }
                    if (nmea_gps_fix.valid.location && nmea_gps_fix.valid.date && nmea_gps_fix.valid.time && (nmea_gps_fix.satellites > 3) ) {
                        Serial.println(F("Fix done."));
                        fix_done = true;
                    }
                }
                GpsOff();
                break;
        case 'f':
                the_fix.latitude = random(-800000, 800000) / 10000.0;
                the_fix.longitude = random(-1800000, 1800000) / 10000.0;
                the_fix.year = 2016 + random(5); // any year b/w 2016 and 2020
                the_fix.month = 1 + random(12);
                the_fix.day = 1 + random(31); // will sometimes give bogus dates
                the_fix.hour = 1 + random(23);
                the_fix.minute = 1 + random(59);
                the_fix.second = 1 + random(59);
                the_fix.satellites = 1;
                //the_fix.fixAcquired = true;
                //staleFix = false;
                break;
        }
        return the_fix;
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
