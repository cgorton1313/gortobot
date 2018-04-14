/* This test outputs voltage and status of a real battery */

#include "configs/consts.h"
#include "configs/includes.h"
#include <Arduino.h>

uint16_t runNum;
uint16_t loopCount = 0;
const uint16_t BATTERY_TEST_WAIT_TIME = 6;
uint32_t interval = 900;

GbFix fix;
GbRealBattery battery1 =
    GbRealBattery(1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY_TEST_WAIT_TIME, BATTERY_VOLTAGE_PIN);
GbRealBattery battery2 =
    GbRealBattery(2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  BATTERY_TEST_WAIT_TIME, BATTERY2_VOLTAGE_PIN);
GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);
GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);
GbMessageHandler messageHandler = GbMessageHandler();

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

  // Get a position
  // GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  // fix = gb_gps.GetFix();

  // Construct the outbound message as a string
  String logSentence = messageHandler.BuildOutboundMessage(
      MESSAGE_VERSION, runNum, loopCount, fix, battery1.GetVoltage(),
      battery2.GetVoltage(), 181, messageHandler.GetDiagnosticMessage());
  Serial.println(logSentence);

  // Use the wifi module to transmit the outbound message
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  uint8_t wifi_attempt = 1;
  bool wifi_successful = false;
  while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
    if (gb_wifi.UseWifi(logSentence)) {
      wifi_successful = true;
    }
    wifi_attempt++;
  }

  GbUtility::GortoNap(interval);
}
