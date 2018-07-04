/* This test exercises the sleep functionality */

#include "../../src/configs/consts.h"
#include "../../src/configs/consts.h"
#include "../../src/configs/includes.h"
#include <Arduino.h>

static GbFix fix;

GbBlinker blinker(LED_PIN);
static GbAirSensor airSensor(AIR_SENSOR_POWER_PIN);
static GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);
static GbRealBattery battery1 =
    GbRealBattery(1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY_VOLTAGE_PIN);
static GbRealBattery battery2 =
    GbRealBattery(2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY2_VOLTAGE_PIN);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Battery wait test 2 starting..."));
}

void loop() {
  delay(1000);
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  delay(1000);

  GbUtility::GortoNap(6);

  blinker.Blink(2);
}
