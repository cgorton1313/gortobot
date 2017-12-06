#include <Arduino.h>
#include <NMEAGPS.h>
#include "gb_fix.h"

#ifndef gb_gps_h
#define gb_gps_h

class GbGps {

private:
    byte _gps_power_pin1;
    byte _gps_power_pin2;
    HardwareSerial* _gps_port;
    void GpsOn();
    void GpsOff();
    //NMEAGPS _gps;
    //gps_fix _fix; // this is a NeoGPS fix

public:
    //NMEAGPS _gps;
    GbGps(byte power_pin1, byte power_pin2, HardwareSerial &port, unsigned long baud);
    GbFix GetFix(char fix_type);
};

#endif
