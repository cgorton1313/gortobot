/*
This file defines all the pins and sets inital state.
Useful for switching hardware targets.
*/

#include <Arduino.h>

#ifndef GB_PINS_H
#define GB_PINS_H

// Pin assignments
#define GPS_SERIAL_PORT Serial1
#define WIFI_SERIAL_PORT Serial2
#define SATCOM_SERIAL_PORT Serial3
const uint8_t LED_PIN = LED_BUILTIN; // to external LED via 420 ohm resistor
const uint8_t GPS_POWER_PIN_1 = 42;
const uint8_t GPS_POWER_PIN_2 = 43;
const uint8_t BATTERY_VOLTAGE_PIN = A0;      // yellow
const uint8_t BATTERY2_VOLTAGE_PIN = A2;     // white
const uint8_t SAIL_POSITION_SENSOR_PIN = A5; // green
const uint8_t SAIL_POSITION_ENABLE_PIN = 28; // orange
const uint8_t MOTOR_POWER_ENABLE_PIN = 39;   // blue
const uint8_t MOTOR_DIRECTION_PIN = 29;      // brown (to Phase on driver)
const uint8_t MOTOR_SPEED_PIN = 46;          // green (to ENABLE on driver)
const uint8_t CHIP_SELECT = 10;              // temp while using only satellite?
const uint8_t SATELLITE_SLEEP_PIN = 7;
const uint8_t WIFI_ENABLE_PIN = 40;
const uint8_t RANDOM_SEED_PIN = A7; // leave unattached
const uint8_t AIR_SENSOR_POWER_PIN = 6;

void initPins() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(GPS_POWER_PIN_1, OUTPUT);
  pinMode(GPS_POWER_PIN_2, OUTPUT);
  digitalWrite(GPS_POWER_PIN_1, LOW);
  digitalWrite(GPS_POWER_PIN_2, LOW);

  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  pinMode(BATTERY2_VOLTAGE_PIN, INPUT);

  pinMode(SAIL_POSITION_SENSOR_PIN, INPUT);

  pinMode(SAIL_POSITION_ENABLE_PIN, OUTPUT);
  digitalWrite(SAIL_POSITION_ENABLE_PIN, LOW);

  pinMode(MOTOR_POWER_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR_SPEED_PIN, OUTPUT);
  digitalWrite(MOTOR_POWER_ENABLE_PIN, LOW);
  digitalWrite(MOTOR_DIRECTION_PIN, LOW);
  digitalWrite(MOTOR_SPEED_PIN, LOW);

  pinMode(SATELLITE_SLEEP_PIN, OUTPUT);
  digitalWrite(SATELLITE_SLEEP_PIN, LOW);

  pinMode(WIFI_ENABLE_PIN, OUTPUT);
  digitalWrite(WIFI_ENABLE_PIN, LOW);

  pinMode(RANDOM_SEED_PIN, INPUT);

  pinMode(AIR_SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(AIR_SENSOR_POWER_PIN, LOW);
}

#endif
