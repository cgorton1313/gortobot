/* Gortobot v3c

The main.cpp program handles the necessary includes, pin assignments, global
consts & variables, etc.
Then, after setup, it runs the loop. The loop simply collects data,
sends data and receives instructions, then executes the received orders.
Rinse and repeat.

There are some global housekeeping functions here as well.

*/

// TODO: store orders in EEPROM?

// Includes
#include "configs/includes.h"

// Constants
#include "configs/consts.h"

// Global variables
static uint16_t runNum;        // increments each time the device starts
static uint16_t loopCount = 0; // increments at each loop
static GbFix fix;              // last known position and time of the vessel
bool rxMessageInvalid = false;
bool tackIsA = true; // keeps track of which tack we're on
uint32_t currentTackTime = 0;

// Global structs
static GbTrimResult trimResult = {.success = true,
                                  .sailStuck = false,
                                  .trimRoutineExceededMax = false,
                                  .sailBatteryTooLow = false};

static GbSailingOrders sailingOrders = {.loggingInterval = 12 * 60,
                                        .orderedSailPositionA = 10,
                                        .orderedTackTimeA = 24 * 60,
                                        .orderedSailPositionB = 90,
                                        .orderedTackTimeB = 12 * 60};

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
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE, TRIM_ROUTINE_MAXIMUM_SECONDS,
                   MAST_POSITION_CALIBRATION);
static GbMessageHandler messageHandler = GbMessageHandler();

void waitForBatteries() {
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

  trimResult = {.success = true,
                .sailStuck = false,
                .trimRoutineExceededMax = false,
                .sailBatteryTooLow = false};

  if (battery2.GetVoltage() < MINIMUM_BATTERY_VOLTAGE) {
    trimResult.success = false;
    trimResult.sailBatteryTooLow = true;
  }

  uint32_t elapsedTime = 0; // used to track seconds during sail operation
  int16_t currentOrderedSailPosition;

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

    if (trimResult.success) {
      if (abs(sail.GetSailPosition() - currentOrderedSailPosition) > 5) {
        trimResult = sail.Trim(currentOrderedSailPosition);
      } else {
        DEBUG_PRINTLN(F("Close enough, no need to trim."));
      }
    } else {
      DEBUG_PRINTLN(F("Unsuccessful sail trim."));
    }

    delay(DELAY_FOR_SERIAL);
    blinker.Blink(2);
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

  if (RESET_EEPROM) {
    GbUtility::ClearEEPROM();
  }

  runNum = GbUtility::IncrementRunNum();
  DEBUG_PRINT(F("Starting runNum "));
  DEBUG_PRINTLN(runNum);

  gb_satcom.SetUpSat(SAT_CHARGE_TIME, ISBD_TIMEOUT);

  sleeper.sleepDelay(1000); // so the subsequent sleep works
  delay(500);
}

void loop() {
  loopCount++;

  // Get a position
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  fix = gb_gps.GetFix();

  // Construct the outbound message as a string
  // TODO: GbTrimResult and rx
  bool rxMessageInvalid = true;
  GbTrimResult trimResult = {.success = true,
                             .sailStuck = false,
                             .trimRoutineExceededMax = false,
                             .sailBatteryTooLow = false};
  String logSentence = messageHandler.BuildOutboundMessage(
      MESSAGE_VERSION, runNum, loopCount, fix, battery1.GetVoltage(),
      battery2.GetVoltage(), sail.GetSailPosition(),
      messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid));

  // Optionally use the wifi module to transmit the outbound message
  if (USING_WIFI) {
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
    uint8_t wifi_attempt = 1;
    bool wifi_successful = false;
    while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
      if (gb_wifi.UseWifi(logSentence)) {
        wifi_successful = true;
      }
      wifi_attempt++;
    }
  }

  // Send/receive message via satcom
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  String inboundMessage = "";
  bool txSuccess = false;
  if (gb_satcom.UseSatcom(logSentence)) {
    txSuccess = true;
    inboundMessage = gb_satcom.GetInboundMessage();
    DEBUG_PRINT(F("Received inbound message of: "));
    DEBUG_PRINTLN(inboundMessage);
  }

  // Parse inbound message
  if (txSuccess) {
    sailingOrders = messageHandler.ParseMessage(inboundMessage, sailingOrders);
    thisWatch = loggingInterval; // TODO deprecate this variable?
  } else {
    thisWatch = min(FAILURE_RETRY, loggingInterval);
  }

  // Execute sailing orders for one logging interval
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  watchStander.StandWatch(sail, sailingOrders);
}
