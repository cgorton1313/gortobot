/* Gortobot v3c */
#include "Arduino.h"
#ifndef UNIT_TEST

// TODO: battery classes, abstract, fake
// TODO: gps classes, abstract, fake
// TODO: integrate IridiumSBD 2.0 and test
// TODO: watchdog timer
// TODO: write copious comments
// TODO: combine message listener, parser, sentence builder, etc.

// Includes
#include "communications/gb_blinker.h"
#include "communications/gb_message_handler.h"
#include "communications/gb_sailing_orders.h"
#include "communications/gb_satcom.h"
#include "configs/config.h"
#include "configs/pins.h"
#include "sailing/gb_watch_stander.h"
#include "utilities/gb_utility.h"
#include <gb_library.h>

// Constants
const uint8_t DELAY_FOR_SERIAL = 10; // ms to delay so serial ouput is clean
const uint32_t CONSOLE_BAUD = 115200, WIFI_BAUD = 115200, GPS_BAUD = 38400;
const uint32_t SAT_BAUD = 19200;
const uint16_t SAT_CHARGE_TIME =
    30; // seconds to wait at start-up for super-capacitor
const uint16_t ISBD_TIMEOUT = 600;  // seconds to try getting isbd success
const uint16_t FAILURE_RETRY = 600; // seconds to wait after tx failure
const uint8_t WIFI_ATTEMPT_LIMIT =
    3; // number of times to try connecting to wifi
const float MINIMUM_BATTERY_VOLTAGE =
    3.4; // system will wait for charging at this low voltage threshold
const float BATTERY_OKAY_VOLTAGE =
    3.5; // system will resume program at this voltage threshold
const uint16_t BATTERY_WAIT_TIME =
    2; // seconds to wait between checking for batteryOkay
const uint8_t MESSAGE_VERSION =
    4; // 2 = long form, 3 = base62, 4 = base62 and 2 batteries
const uint16_t MIN_SAIL_ANGLE = 0, MAX_SAIL_ANGLE = 360; // limits for sail
const uint16_t TRIM_ROUTINE_MAXIMUM_SECONDS =
    900; // max number of trim seconds allowed to get to ordered position.
         // testing shows 450 should be max

// Global variables
// TODO: delete this once useSail is gone to cpp
uint32_t loggingInterval = 60; // seconds b/w logging events, 1 day =
// 86,400 secs which is max
uint16_t runNum;        // increments each time the device starts
uint16_t loopCount = 0; // increments at each loop
bool fixAcquired = false, staleFix = true; // for GPS
uint16_t timeSinceLastFramLog = 0;
String logSentence = "";
bool txSuccess;

uint16_t currentTackTime =
    0; // keeps track of how long we've been on current tack in minutes
uint32_t thisWatch;
bool rxMessageInvalid = false;
GbFix fix;
GbSailingOrders sailingOrders = {.sailMode = 'r',
                                 .loggingInterval = 60,
                                 .orderedSailPositionA = 270,
                                 .orderedTackTimeA = 90,
                                 .orderedSailPositionB = 270,
                                 .orderedTackTimeB = 90};

// Objects
GbGps gb_gps = GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_PORT, GPS_BAUD);
GbSatcom gb_satcom = GbSatcom(SATELLITE_SLEEP_PIN, SAT_BAUD);
GbWifi wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_PORT, WIFI_BAUD);
GbBattery battery1 =
    GbBattery(1, BATTERY_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE,
              BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME, CHECKING_VOLTAGE);
GbBattery battery2 =
    GbBattery(2, BATTERY2_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE,
              BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME, CHECKING_VOLTAGE);
GbSentenceBuilder sentence_builder = GbSentenceBuilder(MESSAGE_VERSION);
GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
            MOTOR_POWER_ENABLE_PIN, MOTOR_IN_1_PIN, MOTOR_IN_2_PIN,
            MIN_SAIL_ANGLE, MAX_SAIL_ANGLE, TRIM_ROUTINE_MAXIMUM_SECONDS);
GbWatchStander watchStander = GbWatchStander(LED_PIN);
GbMessageHandler messageHandler = GbMessageHandler();

void setup() {
  randomSeed(analogRead(RANDOM_SEED_PIN)); // for faking data differently each
                                           // run, A7 should be floating

  Serial.begin(CONSOLE_BAUD);

  // Pin Modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(GPS_POWER_PIN_1, OUTPUT);
  pinMode(GPS_POWER_PIN_2, OUTPUT);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);  // not necessary but for clarity
  pinMode(BATTERY2_VOLTAGE_PIN, INPUT); // not necessary but for clarity
  pinMode(SATELLITE_SLEEP_PIN, OUTPUT);
  pinMode(MOTOR_IN_1_PIN, OUTPUT);
  pinMode(MOTOR_IN_2_PIN, OUTPUT);
  pinMode(WIFI_ENABLE_PIN, OUTPUT);

  // Initial pin states
  analogWrite(LED_PIN, LOW);          // turn off LED
  digitalWrite(GPS_POWER_PIN_1, LOW); // turn off GPS
  digitalWrite(GPS_POWER_PIN_2, LOW); // turn off GPS
  digitalWrite(WIFI_ENABLE_PIN, LOW); // turn off wifi
  digitalWrite(MOTOR_POWER_ENABLE_PIN,
               HIGH); // turn off motor driver high is off

  // TODO: check this
  // isbd.sleep(); // turn off ISBD

  if (RESET_EEPROM) {
    GbUtility::ClearEEPROM();
  }

  runNum = GbUtility::IncrementRunNum();
  Serial.print(F("Starting runNum "));
  Serial.println(runNum);

  if (USING_SAT) {
    gb_satcom.SetUpSat(SAT_CHARGE_TIME, ISBD_TIMEOUT);
  }

  delay(1000); // for some forgotten reason
}

void loop() {
  loopCount++;

  if (USING_GPS) {
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
    fix = gb_gps.GetFix('r'); // 'r' = 'real'
  } else {
    fix = gb_gps.GetFix('f'); // 'f' = 'fake'
  }

  logSentence = sentence_builder.Sentence(
      runNum, loopCount, fix, battery1.GetVoltage(), battery2.GetVoltage(),
      sail.GetSailPosition(), messageHandler.GetDiagnosticMessage());

  if (USING_WIFI) {
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
    uint8_t wifi_attempt = 1;
    bool wifi_successful = false;
    while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
      if (wifi.UseWifi(logSentence)) {
        wifi_successful = true;
      }
      wifi_attempt++;
    }
  }

  String inboundMessage = "";
  txSuccess = false;
  if (USING_SAT) {
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
    if (gb_satcom.UseSatcom(logSentence)) {
      txSuccess = true;
      inboundMessage = gb_satcom.GetInboundMessage();
      Serial.print(F("Received mock inbound message of: "));
      Serial.println(inboundMessage);
    }
  } else if (USING_SERIAL_MONITOR_ORDERS) {
    inboundMessage = messageHandler.GetSerialMessage();
    txSuccess = true;
  } else {
    inboundMessage = messageHandler.GetFakeMessage(sailingOrders);
    txSuccess = true;
  }

  // Parse inbound message
  if (txSuccess) {
    sailingOrders = messageHandler.ParseMessage(inboundMessage);
    thisWatch = loggingInterval;
  } else {
    thisWatch = min(FAILURE_RETRY, loggingInterval);
  }

  // Sail for one watch cycle
  sailingOrders.sailMode = sailMode; // for now
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  watchStander.StandWatch(sail, sailingOrders);
}

#endif
