#include <stdint.h>

#ifndef GB_ABSTRACT_BATTERY_H
#define GB_ABSTRACT_BATTERY_H

class GbAbstractBattery {

protected:
  const uint8_t _battery_number;
  const float _min_voltage;
  const float _retry_voltage;

public:
  GbAbstractBattery(uint8_t battery_number, float min_voltage,
                    float retry_voltage);
  virtual float GetVoltage() = 0;
  virtual char Status();
};

#endif
