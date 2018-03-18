#include "gb_battery.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

void test_fake_battery_voltage(void) {
  GbBattery battery = GbBattery(1, 22, 3.4, 3.5, 3, false);
  TEST_ASSERT_EQUAL_FLOAT(3.99, battery.GetVoltage());
}

void test_real_battery_voltage(void) {
  GbBattery battery = GbBattery(1, 22, 3.4, 3.5, 3, true);
  float voltage = battery.GetVoltage();
  TEST_ASSERT(voltage > 0.0 && voltage < 5.0);
}

void test_fake_battery_status(void) {
  GbBattery battery = GbBattery(1, 1, 3.4, 3.5, 3, false);
  TEST_ASSERT_EQUAL('g', battery.Status());
}

void setup() {
  delay(1000); // for proper printing

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_fake_battery_voltage);
  RUN_TEST(test_real_battery_voltage);
  RUN_TEST(test_fake_battery_status);

  UNITY_END();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }

#endif
