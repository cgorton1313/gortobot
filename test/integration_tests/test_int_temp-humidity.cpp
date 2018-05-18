/* This test exercises the fake GPS */

#include "..\lib\Adafruit_HTU21DF_Library\Adafruit_HTU21DF.h"
#include "communications/gb_message_handler.h"
#include "configs/consts.h"
#include "configs/pins.h"
#include "navigation/gb_fix.h"
#include "communications/gb_wifi.h"
#include "utilities/gb_utility.h"
#include "power/gb_real_battery.h"
#include <Arduino.h>

uint16_t runNum;
uint16_t loopCount = 0;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
GbMessageHandler messageHandler = GbMessageHandler();
GbFix fix;
GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);
GbRealBattery battery1 =
    GbRealBattery(1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  60, BATTERY_VOLTAGE_PIN);
GbRealBattery battery2 =
    GbRealBattery(2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE,
                  60, BATTERY2_VOLTAGE_PIN);

void setup() {
  Serial.begin(CONSOLE_BAUD);
  Serial.println("HTU21D-F test");
  pinMode(TEMP_HUMIDITY_POWER_PIN, OUTPUT);
  digitalWrite(TEMP_HUMIDITY_POWER_PIN, HIGH);

  runNum = GbUtility::IncrementRunNum();
  Serial.print(F("Starting runNum "));
  Serial.println(runNum);

  pinMode(WIFI_ENABLE_PIN, OUTPUT);

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1)
      ;
  }
}

void loop() {
  loopCount++;
  String logSentence = messageHandler.BuildOutboundMessage(
      7, runNum, loopCount, fix, battery1.GetVoltage(), battery2.GetVoltage(), 181, messageHandler.GetDiagnosticMessage(),
      htu.readTemperature(), htu.readHumidity());
  Serial.println(logSentence);

  // Use the wifi module to transmit the outbound message
  // GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  uint8_t wifi_attempt = 1;
  bool wifi_successful = false;
  while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
    if (gb_wifi.UseWifi(logSentence)) {
      wifi_successful = true;
    }
    wifi_attempt++;
  }

  delay(500);
}
