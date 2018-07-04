/* This test exercises the full stack except satcom */

#include "../../src/configs/config.h"
#include "../../src/configs/consts.h"
#include "../../src/configs/includes.h"
#include "../../src/configs/pins.h"
#include "../../src/utilities/gb_utility.h"
#include <Arduino.h>
#include <Sleep_n0m1.h>

// Global variables
static uint32_t loggingInterval = 6;
static Sleep sleeper;
static GbBlinker blinker(LED_PIN);
static GbRealBattery battery1 = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_VOLTAGE_PIN);
static GbRealBattery battery2 = GbRealBattery(
    2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);

void setup() {
  initPins();

  sleeper.pwrDownMode();

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Battery Sensor Test starting"));

  delay(1000);
}

void loop() {

  Serial.print("Battery 1: ");
  Serial.print(battery1.GetVoltage());
  Serial.print(" | Battery 2: ");
  Serial.println(battery2.GetVoltage());

  delay(DELAY_FOR_SERIAL);
  sleeper.sleepDelay(loggingInterval * 1000);
  blinker.Blink(2);
}
