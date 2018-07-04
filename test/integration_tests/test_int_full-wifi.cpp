/* This test exercises the full stack except satcom */

#include "../../src/configs/config.h"
#include "../../src/configs/consts.h"
#include "../../src/configs/includes.h"
#include "../../src/configs/pins.h"
#include "../../src/utilities/gb_utility.h"
#include <Arduino.h>
#include <Sleep_n0m1.h>

// Global variables
static uint32_t loggingInterval = 20;
static uint16_t runNum;
static uint32_t loopCount = 0;
static GbFix fix;
static Sleep sleeper;
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

void wait() {
  char battery1Status = battery1.Status();
  char battery2Status = battery2.Status();
  bool batteriesCritical = (battery1Status == 'r' && battery2Status == 'r');

  if (batteriesCritical) {
    DEBUG_PRINTLN(F("Both batteries critical!"));
    while (battery1Status != 'g' && battery2Status != 'g') {
      DEBUG_PRINT(F("Neither battery green. Waiting "));
      DEBUG_PRINT(BATTERY_WAIT_TIME);
      DEBUG_PRINTLN(F(" seconds."));
      delay(DELAY_FOR_SERIAL);

      sleeper.sleepDelay(BATTERY_WAIT_TIME * 1000);

      DEBUG_PRINTLN(F("Wait time elapsed. Retrying."));
      battery1Status = battery1.Status();
      battery2Status = battery2.Status();
    }
  }
}

void setup() {
  initPins();

  sleeper.pwrDownMode();

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Full Wifi Test starting"));

  runNum = GbUtility::IncrementRunNum();
  DEBUG_PRINT(F("Starting runNum "));
  DEBUG_PRINTLN(runNum);
  delay(500);
  sleeper.sleepDelay(1000); // so the subsequent sleep works
  delay(500);
}

void loop() {
  loopCount++;
  DEBUG_PRINTLN(loopCount);

  // Get a position
  wait();
  // fix = gb_gps.GetFix();
  fix = {.latitude = 1.11,
               .longitude = -2.22,
               .year = 2018,
               .month = 6,
               .day = 5,
               .hour = 11,
               .minute = 59,
               .second = 1,
               .satellites = 4};

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
  wait();
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
      order = 10;
    } else {
      order = 30;
    }

    DEBUG_PRINT("sailPosition = ");
    DEBUG_PRINTLN(sail.GetSailPosition());
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

  delay(DELAY_FOR_SERIAL);
  sleeper.sleepDelay(loggingInterval * 1000);
  blinker.Blink(2);
}
