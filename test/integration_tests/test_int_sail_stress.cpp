/* This test exercises the Sail motor */

#include "..\..\lib\Adafruit_HTU21DF_Library\Adafruit_HTU21DF.h"
#include "../../src/communications/gb_message_handler.h"
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
const uint32_t interval = 6;

Adafruit_HTU21DF airSensor = Adafruit_HTU21DF();
GbFix fix;
GbTrimResult trimResult;
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);
GbRealBattery battery1 = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_VOLTAGE_PIN);
GbRealBattery battery2 = GbRealBattery(
    2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);
GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);
GbMessageHandler messageHandler = GbMessageHandler();

void PrintVoltages() {
  DEBUG_PRINT(F("battery1: "));
  DEBUG_PRINT(battery1.GetVoltage());
  DEBUG_PRINT(F(" | battery2: "));
  DEBUG_PRINTLN(battery2.GetVoltage());
}

void setup() {
  DEBUG_BEGIN(CONSOLE_BAUD);

  pinMode(WIFI_ENABLE_PIN, OUTPUT);
  pinMode(TEMP_HUMIDITY_POWER_PIN, OUTPUT);
  digitalWrite(TEMP_HUMIDITY_POWER_PIN, LOW);

  runNum = GbUtility::IncrementRunNum();
  DEBUG_PRINT(F("Starting runNum "));
  DEBUG_PRINTLN(runNum);
  delay(1000);
}

void loop() {
  loopCount++;
  GbUtility::WaitForBatteries(BATTERY_TEST_WAIT_TIME, battery1, battery2);

  if (battery2.GetVoltage() > MINIMUM_BATTERY_VOLTAGE) {
    uint16_t order = (sail.GetSailPosition() + 25) % 360;
    DEBUG_PRINT("Trimming to: ");
    DEBUG_PRINTLN(order);
    trimResult = sail.Trim(order);
  } else {
    DEBUG_PRINTLN(F("Skipping sail routine - not enough voltage."));
    trimResult = {.success = false,
                  .sailStuck = false,
                  .trimRoutineExceededMax = false,
                  .sailBatteryTooLow = true};
  }

  PrintVoltages();

  digitalWrite(TEMP_HUMIDITY_POWER_PIN, HIGH);
  delay(1000);
  float temperature = 0.0;
  float humidity = 0.0;

  if (!airSensor.begin()) {
    DEBUG_PRINTLN("Couldn't find sensor!");
  } else {
    temperature = airSensor.readTemperature();
    humidity = airSensor.readHumidity();
  }
  digitalWrite(TEMP_HUMIDITY_POWER_PIN, LOW);

  String logSentence = messageHandler.BuildOutboundMessage(
      6, runNum, loopCount, fix, battery1.GetVoltage(), battery2.GetVoltage(),
      sail.GetSailPosition(),
      messageHandler.GetDiagnosticMessage(trimResult, false), temperature,
      humidity);
  DEBUG_PRINTLN(logSentence);

  uint8_t wifi_attempt = 1;
  bool wifi_successful = false;
  while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
    if (gb_wifi.UseWifi(logSentence)) {
      wifi_successful = true;
    }
    wifi_attempt++;
  }

  DEBUG_PRINT(F("Napping for "));
  DEBUG_PRINT(interval);
  DEBUG_PRINTLN(F(" seconds."));
  GbUtility::GortoNap(interval);
}
