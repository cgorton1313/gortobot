#include <Arduino.h>
#include <NMEAGPS.h>

class GbGps {

private:
    byte _gps_power_pin1;
    byte _gps_power_pin2;
    HardwareSerial* _gps_port;
    void GpsOn();
    void GpsOff();
    NMEAGPS* _gps;
    gps_fix _fix;

public:
    GbGps(NMEAGPS &the_gps, byte power_pin1, byte power_pin2, HardwareSerial &port, unsigned long baud);
    void GetFix(char fix_type);
};
