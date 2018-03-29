#include "power/gb_battery.h"
#include <Arduino.h>
#include <unity.h>

#ifdef UNIT_TEST

// The fake battery should always return a voltage of 3.99
void test_fake_battery_voltage(void) {
        GbBattery battery = GbBattery(1, 22, 3.4, 3.5, 3, false);
        TEST_ASSERT_EQUAL_FLOAT(3.99, battery.GetVoltage());
}

// A real battery should always return a voltage between 0.0-5.0
void test_real_battery_voltage(void) {
        GbBattery battery = GbBattery(1, 22, 3.4, 3.5, 3, true);
        float voltage = battery.GetVoltage();
        TEST_ASSERT(voltage > 0.0 && voltage < 5.0);
}

// Since the fake battery is always 3.99, the status is always 'g' (green)
void test_fake_battery_status(void) {
        GbBattery battery = GbBattery(1, 1, 3.4, 3.5, 3, false);
        TEST_ASSERT_EQUAL('g', battery.Status());
}

// A real battery must always have a status of 'g', 'y', or 'r' (green/yellow/red)
void test_real_battery_status(void) {
        GbBattery battery = GbBattery(1, 1, 3.4, 3.5, 3, true);
        char actualStatus = battery.Status();
        TEST_ASSERT(actualStatus == 'r' || actualStatus == 'g' || actualStatus == 'y');
}

void setup() {
        delay(1000); // for proper printing

        UNITY_BEGIN();

        RUN_TEST(test_fake_battery_voltage);
        RUN_TEST(test_real_battery_voltage);
        RUN_TEST(test_fake_battery_status);
        RUN_TEST(test_real_battery_status);

        UNITY_END();

        pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
        // Blink the LED when tests are done
        digitalWrite(LED_BUILTIN, (millis() / 500) % 2);
}

#endif
