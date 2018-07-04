#include "gb_real_battery.h"
#include <Sleep_n0m1.h>

float GbRealBattery::GetVoltage() {
  delay(1000); // settle down
  uint32_t battery_voltage_int = 0;
  float vccSum = 0.0;
  Vcc vcc(5.12/5.04); // VccCorrection

  const uint8_t samples = 50; // number of samples to take (50 is best from testing)
  for (uint8_t i = 0; i < samples; i++) {
    delay(20); // 20ms is best from testing
    battery_voltage_int += analogRead(_pin);
    vccSum += vcc.Read_Volts();
  }

  return (vccSum/samples * (((float)battery_voltage_int / samples) / 1023.0));
}

char GbRealBattery::Status() {
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
