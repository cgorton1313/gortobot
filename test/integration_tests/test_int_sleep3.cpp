/* This test exercises the sleep functionality */

#include "../../src/configs/consts.h"
#include "../../src/configs/includes.h"
#include <Arduino.h>

static GbRealBattery battery = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN(F("Sleep test 3 starting..."));
}

void loop() {
  // GbUtility::GortoNap(60);
  GbUtility::WaitForBatteries(60, battery, battery);
}
