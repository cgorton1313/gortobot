#include <Arduino.h>
#include <NMEAGPS.h>
#include "gb_fix.h"

#ifndef gb_gps_h
#define gb_gps_h

class GbGps {

private:
    uint8_t _gps_power_pin1;
    uint8_t _gps_power_pin2;
    HardwareSerial* _gps_port;
    void GpsOn();
    void GpsOff();
    //NMEAGPS _gps;
    //gps_fix _fix; // this is a NeoGPS fix

public:
    //NMEAGPS _gps;
    GbGps(uint8_t power_pin1, uint8_t power_pin2, HardwareSerial &port, uint32_t baud);
    GbFix GetFix(char fix_type);
};

#endif
