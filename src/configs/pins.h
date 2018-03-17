#include <Arduino.h>

// Pin assignments
#define GPS_PORT Serial1
#define WIFI_PORT Serial2
#define ISBD_PORT Serial3
const uint8_t LED_PIN = 13; // brown, to external LED via 420 ohm resistor
const uint8_t GPS_POWER_PIN_1 = 26;
const uint8_t GPS_POWER_PIN_2 = 25;
const uint8_t BATTERY_VOLTAGE_PIN = A0;      // green
const uint8_t BATTERY2_VOLTAGE_PIN = A2;     // yellow
const uint8_t SAIL_POSITION_SENSOR_PIN = A5; // green
const uint8_t SAIL_POSITION_ENABLE_PIN = 50; // red
const uint8_t MOTOR_POWER_ENABLE_PIN = 32;
const uint8_t MOTOR_IN_1_PIN = 30, MOTOR_IN_2_PIN = 31; // on a JST under the board
const uint8_t CHIP_SELECT = 10;        // temp while using only satellite
const uint8_t SATELLITE_SLEEP_PIN = 7; // 7-green
const uint8_t WIFI_ENABLE_PIN = 4;     // brown
const uint8_t RANDOM_SEED_PIN = A7;
