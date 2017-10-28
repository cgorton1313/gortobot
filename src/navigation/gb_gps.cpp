#include <Arduino.h>
#include "navigation/gb_gps.h"
#include <Narcoleptic.h>

GbGps::GbGps(NMEAGPS &the_gps, byte power_pin1, byte power_pin2, HardwareSerial &port, unsigned long baud) {
        _gps_power_pin1 = power_pin1;
        _gps_power_pin2 = power_pin2;
        // try defining _gps_port first, then .begin it
        port.begin(baud);
        _gps_port = &port;
        _gps = &the_gps;
}

void GbGps::GetFix(char fix_type) {
        bool fix_done;
        switch (fix_type) {
        case 'r':
                delay(2005); // to trigger staleFix because millis stop during sleep
                GpsOn();
                fix_done = false;
                while (!fix_done)
                {
                    //while (true)
                    while (_gps->available(Serial1)) // see if we can pass this in
                    {
                        _fix = _gps->read();
                        if (_fix.valid.location && _fix.valid.date && _fix.valid.time && (_fix.satellites > 3) ) {
                            Serial.print("Fix: ");
                            Serial.print(_fix.latitude(), 4);
                            //latitude = fix.latitude();
                            Serial.print(", ");
                            Serial.print(_fix.longitude(), 4);
                            //longitude = fix.longitude();
                            Serial.print(", ");
                            NeoGPS::time_t & dt = _fix.dateTime;
                            Serial << dt;
                            //year = fix.dateTime.year;
                            //month = fix.dateTime.month;
                            //day = fix.dateTime.date;
                            //hour = fix.dateTime.hours;
                            //minute = fix.dateTime.minutes;
                            //second = fix.dateTime.seconds;
                            Serial.print(", ");
                            Serial.print(_fix.satellites);
                            Serial.println();
                            fix_done = true;
                        }
                    }
                }
                GpsOff();
                break;
        case 'f':
                //latitude = random(-800000, 800000) / 10000.0;
                //longitude = random(-1800000, 1800000) / 10000.0;
                //year = 2016 + random(5); // any year b/w 2016 and 2020
                //month = 1 + random(12);
                //day = 1 + random(31); // will sometimes give bogus dates
                //hour = 1 + random(23);
                //minute = 1 + random(59);
                //second = 1 + random(59);
                //satellites = 1;
                //fixAcquired = true;
                //staleFix = false;
                break;
        }
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
