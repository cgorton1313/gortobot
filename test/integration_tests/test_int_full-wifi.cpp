/* This test exercises the full stack except satcom */

#include "configs/consts.h"
#include "configs/includes.h"
#include <Arduino.h>

// Global variables
static uint32_t loggingInterval = 60;
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
static GbRealBattery battery1 = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_VOLTAGE_PIN);
static GbRealBattery battery2 = GbRealBattery(
    2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);
static GbWatchStander watchStander = GbWatchStander(LED_PIN);
static GbMessageHandler messageHandler = GbMessageHandler();
static GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);

void setup() {
  Serial.begin(CONSOLE_BAUD);
  Serial.println(F("Full Wifi Test starting"));

  pinMode(LED_BUILTIN, OUTPUT);

  runNum = GbUtility::IncrementRunNum();
  Serial.print(F("Starting runNum "));
  Serial.println(runNum);
  delay(1000);
}

void loop() {
  loopCount++;
  Serial.println(loopCount);

  // Get a position
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  fix = gb_gps.GetFix();
  // fix = {.latitude = 1.11,
  //              .longitude = -2.22,
  //              .year = 2018,
  //              .month = 6,
  //              .day = 5,
  //              .hour = 11,
  //              .minute = 59,
  //              .second = 1,
  //              .satellites = 4};

  // Construct the outbound message as a string
  // TODO: GbTrimResult and rx
  bool rxMessageInvalid = false;
  GbTrimResult trimResult = {.success = true,
                             .sailStuck = false,
                             .trimRoutineExceededMax = false,
                             .sailBatteryTooLow = false};
  GbAirStats airStats = airSensor.GetAirStats();
  String logSentence = messageHandler.BuildOutboundMessage(
      MESSAGE_VERSION, runNum, loopCount, fix, battery1.GetVoltage(),
      battery2.GetVoltage(), sail.GetSailPosition(),
      messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid),
      airStats.temperature, airStats.humidity);

  // use the wifi module to transmit the outbound message
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  uint8_t wifi_attempt = 1;
  bool wifi_successful = false;
  while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
    if (gb_wifi.UseWifi(logSentence)) {
      wifi_successful = true;
    }
    wifi_attempt++;
  }

  if (battery2.GetVoltage() > MINIMUM_BATTERY_VOLTAGE) {
    int16_t order;
    if (loopCount % 2 == 0) {
      order = 0;
    } else {
      order = 20;
    }

    Serial.print("sailPosition = ");
    Serial.println(sail.GetSailPosition());
    Serial.print("Trimming to: ");
    Serial.println(order);
    trimResult = sail.Trim(order);
  } else {
    Serial.println(F("Skipping sail routine - not enough voltage."));
    trimResult = {.success = false,
                  .sailStuck = false,
                  .trimRoutineExceededMax = false,
                  .sailBatteryTooLow = true};
  }

  GbUtility::GortoNap(6);
  blinker.Blink(2);
}
