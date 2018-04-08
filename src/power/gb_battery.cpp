#include "gb_battery.h"
#include <Sleep_n0m1.h>

float GbBattery::GetVoltage() {
  Serial.println("Checking a real voltage.");
  delay(1000); // settle down
  uint16_t battery_voltage_int = 0;
  const uint8_t samples = 10; // number of samples to take
  for (uint8_t i = 0; i < samples; i++) {
    delay(5);
    battery_voltage_int = battery_voltage_int + analogRead(_pin);
  }
  Vcc vcc(1); // VccCorrection
  return (vcc.Read_Volts() * (((float)battery_voltage_int / samples) / 1023.0));
}

char GbBattery::Status() {
  Serial.println("real status");
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
