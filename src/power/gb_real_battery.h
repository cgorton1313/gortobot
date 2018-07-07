/*
This class allows any number of battery objects to be used by Gortobot. Useful
when we switched from 1 bank to 2.
*/

#include "gb_abstract_battery.h"
#include <Vcc.h>

#ifndef GB_BATTERY_H
#define GB_BATTERY_H

class GbRealBattery : public GbAbstractBattery {

private:
  const uint8_t _pin;

public:
  GbRealBattery(uint8_t battery_number, float min_voltage, float retry_voltage,
                uint8_t pin)
      : GbAbstractBattery(battery_number, min_voltage, retry_voltage),
        _pin(pin){};
  virtual float GetVoltage();
  virtual char Status();
};

#endif
