/* This test exercises the single battery */

#include "../../src/configs/consts.h"
#include "../../src/configs/includes.h"
#include <Arduino.h>

// Global variables
static uint32_t loggingInterval = 900;
static uint16_t runNum;
static uint32_t loopCount = 0;
static GbFix fix;
static GbSailingOrders sailingOrders = {.loggingInterval = 60,
                                        .orderedSailPositionA = 270,
                                        .orderedTackTimeA = 90,
                                        .orderedSailPositionB = 270,
                                        .orderedTackTimeB = 90};

static GbBlinker blinker(LED_PIN);
static GbAirSensor airSensor(AIR_SENSOR_POWER_PIN);
static GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);
static GbRealBattery battery = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);
static GbWatchStander watchStander = GbWatchStander(LED_PIN);
static GbMessageHandler messageHandler = GbMessageHandler();
static GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);

void setup() {
  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Full Wifi Single Battery Test starting"));

  pinMode(LED_BUILTIN, OUTPUT);

  runNum = GbUtility::IncrementRunNum();
  DEBUG_PRINT(F("Starting runNum "));
  DEBUG_PRINTLN(runNum);
  delay(1000);
}

void loop() {
  loopCount++;

  // Get a position
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery, battery);
  fix = gb_gps.GetFix();

  // Construct the outbound message as a string
  // TODO: GbTrimResult and rx
  bool rxMessageInvalid = false;
  GbTrimResult trimResult = {.success = true,
                             .sailStuck = false,
                             .trimRoutineExceededMax = false,
                             .sailBatteryTooLow = false};
  GbAirStats airStats = airSensor.GetAirStats();
  String logSentence = messageHandler.BuildOutboundMessage(
      MESSAGE_VERSION, runNum, loopCount, fix, battery.GetVoltage(), 3.2,
      sail.GetSailPosition(),
      messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid),
      airStats.temperature, airStats.humidity);

  // use the wifi module to transmit the outbound message
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery, battery);
  uint8_t wifi_attempt = 1;
  bool wifi_successful = false;
  while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
    if (gb_wifi.UseWifi(logSentence)) {
      wifi_successful = true;
    }
    wifi_attempt++;
  }

  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery, battery);
  int16_t order;
  if (loopCount % 2 == 0) {
    order = 0;
  } else {
    order = 20;
  }

  DEBUG_PRINT("sailPosition = ");
  DEBUG_PRINTLN(sail.GetSailPosition());
  DEBUG_PRINT("Trimming to: ");
  DEBUG_PRINTLN(order);
  trimResult = sail.Trim(order);

  GbUtility::GortoNap(loggingInterval);
}
