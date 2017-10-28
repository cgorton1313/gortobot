#include <Arduino.h>
#include "navigation/gb_gps.h"
#include <Narcoleptic.h>

GbGps::GbGps(byte power_pin1, byte power_pin2, HardwareSerial &port, unsigned long baud) {
        _gps_power_pin1 = power_pin1;
        _gps_power_pin2 = power_pin2;
        _gps_port = &port;
        _gps_port->begin(baud);
}

GbFix GbGps::GetFix(char fix_type) {
        bool fix_done;
        GbFix the_fix;
        switch (fix_type) {
        case 'r':
                delay(2005); // to trigger staleFix because millis stop during sleep
                GpsOn();
                fix_done = false;
                while (!fix_done)
                {
                    while (_gps->available(Serial1)) // see if we can pass this in
                    {
                        _fix = _gps->read();
                        if (_fix.valid.location && _fix.valid.date && _fix.valid.time && (_fix.satellites > 3) ) {
                            Serial.print("Fix: ");
                            Serial.print(_fix.latitude(), 4);
                            the_fix.latitude = _fix.latitude();
                            Serial.print(", ");
                            Serial.print(_fix.longitude(), 4);
                            the_fix.longitude = _fix.longitude();
                            Serial.print(", ");
                            NeoGPS::time_t & dt = _fix.dateTime;
                            Serial << dt;
                            the_fix.year = _fix.dateTime.year;
                            the_fix.month = _fix.dateTime.month;
                            the_fix.day = _fix.dateTime.date;
                            the_fix.hour = _fix.dateTime.hours;
                            the_fix.minute = _fix.dateTime.minutes;
                            the_fix.second = _fix.dateTime.seconds;
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
