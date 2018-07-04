/* This test exercises the SatCom */

// Includes
#include "../../src/communications/gb_message_handler.h"
#include "../../src/configs/config.h"
#include "../../src/configs/includes.h"
#include "../../src/configs/pins.h"
#include "../../src/power/gb_abstract_battery.h"
#include "../../src/power/gb_real_battery.h"
#include "../../src/utilities/gb_utility.h"
#include <Arduino.h>
#include <Sleep_n0m1.h>

// Constants
#include "configs/consts.h"

// Global variables
static uint16_t runNum;        // increments each time the device starts
static uint16_t loopCount = 0; // increments at each loop
static GbFix fix;
GbTrimResult trimResult;
static GbSailingOrders sailingOrders = {.loggingInterval = 30 * 60,
                                        .orderedSailPositionA = 90,
                                        .orderedTackTimeA = 15 * 60,
                                        .orderedSailPositionB = 270,
                                        .orderedTackTimeB = 15};

// Objects
static Sleep sleeper;
static GbBlinker blinker(LED_PIN);
static GbAirSensor airSensor(AIR_SENSOR_POWER_PIN);
static GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);
static GbRealBattery battery1 = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_VOLTAGE_PIN);
static GbRealBattery battery2 = GbRealBattery(
    2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);
static GbSatcom gb_satcom =
    GbSatcom(SATELLITE_SLEEP_PIN, SATCOM_SERIAL_PORT, SAT_BAUD);
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);
static GbMessageHandler messageHandler = GbMessageHandler();

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

void standWatch() {
  DEBUG_PRINT(F("Sailing for "));
  DEBUG_PRINT(sailingOrders.loggingInterval);
  DEBUG_PRINTLN(F(" seconds."));

  DEBUG_PRINT(F("Tack A for "));
  DEBUG_PRINT(sailingOrders.orderedTackTimeA);
  DEBUG_PRINT(F(" seconds at sail position "));
  DEBUG_PRINTLN(sailingOrders.orderedSailPositionA);

  DEBUG_PRINT(F("Tack B for "));
  DEBUG_PRINT(sailingOrders.orderedTackTimeB);
  DEBUG_PRINT(F(" seconds at sail position "));
  DEBUG_PRINTLN(sailingOrders.orderedSailPositionB);

  bool tackIsA = true;      // keeps track of which tack we're on
  uint32_t elapsedTime = 0; // used to track seconds during sail operation
  uint32_t currentTackTime = 0;
  uint16_t currentOrderedSailPosition;

  while (elapsedTime < sailingOrders.loggingInterval) {
    if ((tackIsA && currentTackTime >= sailingOrders.orderedTackTimeA) ||
        (!tackIsA && currentTackTime >= sailingOrders.orderedTackTimeB)) {
      tackIsA = !tackIsA; // change tacks
      currentTackTime = 0;
      DEBUG_PRINTLN(F("Changing tacks"));
    }

    if (tackIsA) {
      currentOrderedSailPosition = sailingOrders.orderedSailPositionA;
      DEBUG_PRINTLN(F("Sailing on tack A"));
    } else {
      currentOrderedSailPosition = sailingOrders.orderedSailPositionB;
      DEBUG_PRINTLN(F("Sailing on tack B"));
    }

    if (sail.ValidOrders(currentOrderedSailPosition)) {
      trimResult = sail.Trim(currentOrderedSailPosition);
    } else {
      DEBUG_PRINTLN(F("Invalid sail position order."));
      trimResult = {.success = false,
                    .sailStuck = false,
                    .trimRoutineExceededMax = false,
                    .sailBatteryTooLow = false,
                    .invalidSailPositionOrder = true};
    }

    delay(DELAY_FOR_SERIAL);
    sleeper.sleepDelay(6000);
    currentTackTime += 6;
    elapsedTime += 6;

    DEBUG_PRINT(F("elapsedTime = "));
    DEBUG_PRINT(elapsedTime);
    DEBUG_PRINT(F(" | currentTackTime = "));
    DEBUG_PRINTLN(currentTackTime);
  }
}

void setup() {
  initPins();
  sleeper.pwrDownMode(); // lowest power setting for sleeping

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Satcom test starting..."));

  runNum = GbUtility::IncrementRunNum();
  DEBUG_PRINT(F("Starting runNum "));
  DEBUG_PRINTLN(runNum);

  // uncomment for real program
  // gb_satcom.SetUpSat(SAT_CHARGE_TIME, ISBD_TIMEOUT);

  delay(500);
  sleeper.sleepDelay(1000); // so the subsequent sleep works
  delay(500);
}

void loop() {
  loopCount++;
  DEBUG_PRINT(F("loopCount = "));
  DEBUG_PRINTLN(loopCount);

  // Construct the outbound message as a string with fake values
  // TODO: rx valid
  bool rxMessageInvalid = false;
  GbTrimResult trimResult = {.success = true,
                             .sailStuck = false,
                             .trimRoutineExceededMax = false,
                             .sailBatteryTooLow = false};
  GbFix fix = {.latitude = 1.11,
               .longitude = -2.22,
               .year = 2018,
               .month = 6,
               .day = 5,
               .hour = 11,
               .minute = 59,
               .second = 1,
               .satellites = 4};

  String logSentence = messageHandler.BuildOutboundMessage(
      MESSAGE_VERSION, runNum, loopCount, fix, battery1.GetVoltage(),
      battery2.GetVoltage(), sail.GetSailPosition(),
      messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid), 20.0,
      40.0);
  DEBUG_PRINT(F("logSentence = "));
  DEBUG_PRINTLN(logSentence);

  // Send/receive message via satcom
  wait();
  String inboundMessage = "";
  bool txSuccess = false;
  if (gb_satcom.UseSatcom(logSentence)) {
    txSuccess = true;
    inboundMessage = gb_satcom.GetInboundMessage();
    DEBUG_PRINT(F("Received inbound message of: "));
    DEBUG_PRINTLN(inboundMessage);
  } else {
    txSuccess = false;
    DEBUG_PRINTLN(F("SatCom transmission failed."));
  }

  wait();
  standWatch();
}
