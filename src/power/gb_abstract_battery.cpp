#include "gb_abstract_battery.h"

GbAbstractBattery::GbAbstractBattery(uint8_t battery_number, float min_voltage,
                                     float retry_voltage, uint16_t wait_time)
    : _battery_number(battery_number), _min_voltage(min_voltage),
      _retry_voltage(retry_voltage), _wait_time(wait_time) {}

char GbAbstractBattery::Status() {
  float voltage_now = GetVoltage();
  char status;
  if (voltage_now < _min_voltage) {
    status = 'r'; // red, bad
  } else if (voltage_now > _retry_voltage) {
    status = 'g'; // green, good
  } else {
    status = 'y'; // yellow, wait a bit
  }
  return status;
}
