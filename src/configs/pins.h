#include <Arduino.h>

// Pin assignments
#define GPS_PORT Serial1
#define WIFI_PORT Serial2
#define ISBD_PORT Serial3
const byte LED_PIN = 13; // brown, to external LED via 420 ohm resistor
const byte GPS_POWER_PIN_1 = 26;
const byte GPS_POWER_PIN_2 = 25;
const byte BATTERY_VOLTAGE_PIN = A0;      // green
const byte BATTERY2_VOLTAGE_PIN = A2;     // yellow
const byte SAIL_POSITION_SENSOR_PIN = A5; // green
const byte SAIL_POSITION_ENABLE_PIN = 50; // red
const byte MOTOR_POWER_ENABLE_PIN = 32;
const byte MOTOR_IN_1_PIN = 30, MOTOR_IN_2_PIN = 31; // on a JST under the board
const byte CHIP_SELECT = 10;        // temp while using only satellite
const byte SATELLITE_SLEEP_PIN = 7; // 7-green
const byte WIFI_ENABLE_PIN = 4;     // brown
const byte RANDOM_SEED_PIN = A7;
