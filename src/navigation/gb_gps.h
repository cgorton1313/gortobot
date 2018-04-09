/*
This is a wrapper class for a GPS object. Allows for easier switching of GPS
libraries,
and handles power methods (on, off, etc.)
*/

#include "gb_abstract_gps.h"
#include "gb_fix.h"
#include <NMEAGPS.h>

#ifndef GB_GPS_H
#define GB_GPS_H

class GbGps : public GbAbstractGps {

private:
  uint8_t _gps_power_pin1;
  uint8_t _gps_power_pin2;
  HardwareSerial *_gps_port;
  void GpsOn();
  void GpsOff();

public:
  GbGps(uint8_t power_pin1, uint8_t power_pin2, HardwareSerial &port,
        uint32_t baud)
      : GbAbstractGps(), _gps_power_pin1(power_pin1),
        _gps_power_pin2(power_pin2), _gps_port(&port) {
    port.begin(baud);
  };

  GbFix GetFix();
};

#endif
