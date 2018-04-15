/* This test exercises the Sail motor */

#include "communications/gb_message_handler.h"
#include "communications/gb_wifi.h"
#include "configs/consts.h"
#include "configs/pins.h"
#include "navigation/gb_fix.h"
#include "power/gb_real_battery.h"
#include "sailing/gb_sail.h"
#include "utilities/gb_utility.h"
#include <Arduino.h>

uint16_t runNum;
uint16_t loopCount = 0;
const uint16_t BATTERY_TEST_WAIT_TIME = 6;
uint32_t interval = 30;

GbFix fix;
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_IN_1_PIN, MOTOR_IN_2_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);
GbRealBattery battery1 =
    GbRealBattery(1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY_TEST_WAIT_TIME, BATTERY_VOLTAGE_PIN);
GbRealBattery battery2 =
    GbRealBattery(2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY_TEST_WAIT_TIME, BATTERY2_VOLTAGE_PIN);
GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);
GbMessageHandler messageHandler = GbMessageHandler();

void GetVoltages() {
  Serial.print(F("battery1: "));
  Serial.print(battery1.GetVoltage());
  Serial.print(F(" | battery2: "));
  Serial.println(battery2.GetVoltage());
}

void setup() {
  Serial.begin(CONSOLE_BAUD);

  pinMode(WIFI_ENABLE_PIN, OUTPUT);

  runNum = GbUtility::IncrementRunNum();
  Serial.print(F("Starting runNum "));
  Serial.println(runNum);
  delay(1000);
}

void loop() {
  loopCount++;

  if (battery2.GetVoltage() > 3.4) {
    uint16_t order = (sail.GetSailPosition() + 200) % 360;
    Serial.print("Trimming to: ");
    Serial.println(order);
    sail.Trim(order);
  } else {
    Serial.println(F("Skipping sail routine - not enough voltage."));
  }

  GetVoltages();

  String logSentence = messageHandler.BuildOutboundMessage(
      MESSAGE_VERSION, runNum, loopCount, fix, battery1.GetVoltage(),
      battery2.GetVoltage(), sail.GetSailPosition(), messageHandler.GetDiagnosticMessage());
  Serial.println(logSentence);

  uint8_t wifi_attempt = 1;
  bool wifi_successful = false;
  while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
    if (gb_wifi.UseWifi(logSentence)) {
      wifi_successful = true;
    }
    wifi_attempt++;
  }

  Serial.print(F("Napping for "));
  Serial.print(interval);
  Serial.println(F(" seconds."));
  GbUtility::GortoNap(interval);
}
