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
  uint8_t _air_sensor_power_pin;
  void AirSensorOn();
  void AirSensorOff();

public:
  // TODO: consts
  GbAirSensor(uint8_t power_pin) : _air_sensor_power_pin(power_pin) {
    pinMode(power_pin, OUTPUT);
    digitalWrite(power_pin, LOW); // turn off sensor
  };

  GbAirStats GetAirStats();
};

#endif
