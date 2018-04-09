#include "power/gb_real_battery.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

// A real battery should always return a voltage between 0.0-5.0
void test_real_battery_voltage(void) {
  GbRealBattery battery = GbRealBattery(1, 22, 3.4, 3.5, 3);
  float voltage = battery.GetVoltage();
  TEST_ASSERT(voltage > 0.0 && voltage < 5.0);
}

// A real battery must always have a status of 'g', 'y', or 'r'
// (green/yellow/red)
void test_real_battery_status(void) {
  GbRealBattery battery = GbRealBattery(1, 1, 3.4, 3.5, 3);
  char actualStatus = battery.Status();
  TEST_ASSERT(actualStatus == 'r' || actualStatus == 'g' ||
              actualStatus == 'y');
}

void setup() {
  delay(1000); // for proper printing

  UNITY_BEGIN();

  RUN_TEST(test_real_battery_voltage);
  RUN_TEST(test_real_battery_status);

  UNITY_END();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Blink the LED when tests are done
  digitalWrite(LED_BUILTIN, (millis() / 500) % 2);
}

#endif
