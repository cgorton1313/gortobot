/*
This is a wrapper class for a GPS object. Allows for easier switching of GPS
libraries,
and handles power methods (on, off, etc.)
*/

#include "gb_abstract_gps.h"
#include "gb_fix.h"
#include "../../src/configs/config.h"
#include <NMEAGPS.h>

#ifndef GB_GPS_H
#define GB_GPS_H

class GbGps : public GbAbstractGps {

private:
  const uint8_t _gps_power_pin1;
  const uint8_t _gps_power_pin2;
  HardwareSerial *_gps_port;
  void GpsOn();
  void GpsOff();

public:
  GbGps(uint8_t power_pin1, uint8_t power_pin2, HardwareSerial &port,
        uint32_t baud);
  GbFix GetFix();
};

#endif
