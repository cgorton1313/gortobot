#include "gb_air_sensor.h"

GbAirStats GbAirSensor::GetAirStats() {
  Adafruit_HTU21DF htu = Adafruit_HTU21DF();
  GbAirStats airStats;

  AirSensorOn();

  if (htu.begin()) {
    airStats.temperature = htu.readTemperature();
    airStats.humidity = htu.readHumidity();
    Serial.print(F("temperature = "));
    Serial.print(airStats.temperature);
    Serial.print(F(" | humidity = "));
    Serial.println(airStats.humidity);
  } else {
    airStats.temperature = 0.0;
    airStats.humidity = 0.0;
    Serial.println(F("Air sensor failure!"));
  }

  AirSensorOff();
  return airStats;
}

void GbAirSensor::AirSensorOn() {
  digitalWrite(_air_sensor_power_pin, HIGH);
  delay(500);
  Serial.println(F("Air sensor on."));
}

void GbAirSensor::AirSensorOff() {
  digitalWrite(_air_sensor_power_pin, LOW);
  Serial.println(F("Air sensor off."));
}
