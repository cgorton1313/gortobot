#include "gb_battery.h"
#include <Sleep_n0m1.h>

GbBattery::GbBattery(uint8_t battery_number, uint8_t pin, float min_voltage,
                     float retry_voltage, uint16_t wait_time,
                     bool checking_voltage) {
  _battery_number = battery_number;
  _pin = pin;
  _min_voltage = min_voltage;
  _retry_voltage = retry_voltage;
  _wait_time = wait_time;
  _checking_voltage = checking_voltage;
}

float GbBattery::GetVoltage() {
  if (_checking_voltage) {
    delay(1000); // settle down
    uint16_t battery_voltage_int = 0;
    const uint8_t samples = 10; // number of samples to take
    for (uint8_t i = 0; i < samples; i++) {
      delay(5);
      battery_voltage_int = battery_voltage_int + analogRead(_pin);
    }
    Vcc vcc(1); // VccCorrection
    return (vcc.Read_Volts() *
            (((float)battery_voltage_int / samples) / 1023.0));
  } else {
    return 3.99; // a faked voltage
  }
}

char GbBattery::Status() {
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
