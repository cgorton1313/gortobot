/* This test exercises the sleep functionality */

#include "configs/pins.h"
#include "configs/consts.h"
#include "utilities/gb_utility.h"
#include "communications/gb_blinker.h"
#include "power/gb_abstract_battery.h"
#include "power/gb_real_battery.h"
#include <Arduino.h>

GbBlinker blinker(LED_PIN);
static GbRealBattery battery1 =
    GbRealBattery(1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY_VOLTAGE_PIN);
static GbRealBattery battery2 =
    GbRealBattery(2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY2_VOLTAGE_PIN);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("Sleep test starting..."));
}

void loop() {
  delay(1000);
  Serial.print(F("battery1 = "));
  Serial.print(battery1.GetVoltage());
  Serial.print(F(" | battery2 = "));
  Serial.println(battery1.GetVoltage());
  delay(1000);
  
  GbUtility::GortoNap(6);

  blinker.Blink(2);
}
