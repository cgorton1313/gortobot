#include <stdint.h>

#ifndef GB_ABSTRACT_BATTERY_H
#define GB_ABSTRACT_BATTERY_H

class GbAbstractBattery {

protected:
  uint8_t _battery_number;
  float _min_voltage;
  float _retry_voltage;
  uint16_t _wait_time;

public:
  GbAbstractBattery(uint8_t battery_number, float min_voltage,
                    float retry_voltage, uint16_t wait_time);
  virtual float GetVoltage() = 0;
  virtual char Status();
};

#endif
