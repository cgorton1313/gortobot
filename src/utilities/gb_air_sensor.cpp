#include "gb_air_sensor.h"

GbAirStats GbAirSensor::GetAirStats() {
  Adafruit_HTU21DF htu = Adafruit_HTU21DF();
  GbAirStats airStats;

  AirSensorOn();

  if (htu.begin()) {
    airStats.temperature = htu.readTemperature();
    airStats.humidity = htu.readHumidity();
    DEBUG_PRINT(F("temperature = "));
    DEBUG_PRINT(airStats.temperature);
    DEBUG_PRINT(F(" | humidity = "));
    DEBUG_PRINTLN(airStats.humidity);
  } else {
    airStats.temperature = 0.0;
    airStats.humidity = 0.0;
    DEBUG_PRINTLN(F("Air sensor failure!"));
  }

  AirSensorOff();
  return airStats;
}

void GbAirSensor::AirSensorOn() {
  digitalWrite(_air_sensor_power_pin, HIGH);
  delay(500);
  DEBUG_PRINTLN(F("Air sensor on."));
}

void GbAirSensor::AirSensorOff() {
  digitalWrite(_air_sensor_power_pin, LOW);
  DEBUG_PRINTLN(F("Air sensor off."));
}
