/*
This is a wrapper class for a temperature/humidity sensor object.
Allows for easier switching of libraries,
and handles power methods (on, off, etc.)
*/

#include <Adafruit_HTU21DF.h>
#include "gb_air_stats.h"
#include "../../src/configs/config.h"

#ifndef GB_AIR_SENSOR_H
#define GB_AIR_SENSOR_H

class GbAirSensor {

private:
  const uint8_t _air_sensor_power_pin;
  void AirSensorOn();
  void AirSensorOff();

public:
  GbAirSensor(uint8_t power_pin);

  GbAirStats GetAirStats();
};

#endif
