/*
This class allows any number of battery objects to be used by Gortobot. Useful
when we switched from 1 bank to 2.
*/

#include "gb_abstract_battery.h"
#include <Vcc.h>

#ifndef GB_BATTERY_H
#define GB_BATTERY_H

class GbBattery : public GbAbstractBattery {

private:
  uint8_t _pin;

public:
  GbBattery(uint8_t battery_number, float min_voltage, float retry_voltage,
            uint16_t wait_time, uint8_t pin)
      : GbAbstractBattery(battery_number, min_voltage, retry_voltage,
                          wait_time), _pin(pin){};
  virtual float GetVoltage();
  virtual char Status();
};

#endif
