/* Gortobot v3c */

// TODO: battery classes, abstract, fake
// TODO: gps classes, abstract, fake
// TODO: integrate IridiumSBD 2.0 and test
// TODO: watchdog timer
// TODO: write copious comments

#include <Arduino.h>

// Program Modes (config)
#include "configs/config.h"

// Includes
#include "communications/gb_sailing_orders.h"
#include "communications/gb_satcom.h"
#include "utilities/gb_utility.h"
#include <gb_library.h>

// Pin assignments
#define GPS_PORT Serial1
#define WIFI_PORT Serial2
#define ISBD_PORT Serial3
const byte LED_PIN = 13; // brown, to external LED via 420 ohm resistor
const byte GPS_POWER_PIN_1 = 26;
const byte GPS_POWER_PIN_2 = 25;
const byte BATTERY_VOLTAGE_PIN = A0;      // green
const byte BATTERY2_VOLTAGE_PIN = A2;     // yellow
const byte SAIL_POSITION_SENSOR_PIN = A5; // green
const byte SAIL_POSITION_ENABLE_PIN = 50; // red
const byte MOTOR_POWER_ENABLE_PIN = 32;
const byte MOTOR_IN_1_PIN = 30, MOTOR_IN_2_PIN = 31; // on a JST under the board
const byte CHIP_SELECT =
    10; // temp while using only satellite. can't remember why
const byte SATELLITE_SLEEP_PIN = 7; // 7-green
const byte WIFI_ENABLE_PIN = 4;     // brown
const byte RANDOM_SEED_PIN = A7;

// Constants
const byte DELAY_FOR_SERIAL = 10; // ms to delay so serial ouput is clean
const unsigned long CONSOLE_BAUD = 115200, WIFI_BAUD = 115200, GPS_BAUD = 38400;
const unsigned long SAT_BAUD = 19200;
const int SAT_CHARGE_TIME =
    30;                       // seconds to wait at start-up for super-capacitor
const int ISBD_TIMEOUT = 600; // seconds to try getting isbd success
const int FAILURE_RETRY = 600;     // seconds to wait after tx failure
const byte WIFI_ATTEMPT_LIMIT = 3; // number of times to try connecting to wifi
const float MINIMUM_BATTERY_VOLTAGE =
    3.4; // system will wait for charging at this low voltage threshold
const float BATTERY_OKAY_VOLTAGE =
    3.5; // system will resume program at this voltage threshold
const int BATTERY_WAIT_TIME =
    2; // seconds to wait between checking for batteryOkay
const byte MESSAGE_VERSION =
    4; // 2 = long form, 3 = base62, 4 = base62 and 2 batteries
const int MINIMUM_SAIL_ANGLE = 0, MAXIMUM_SAIL_ANGLE = 360; // limits for sail
const int TRIM_ROUTINE_MAXIMUM_SECONDS =
    900; // max number of trim seconds allowed to get to ordered position.
         // testing shows 450 should be max

// Global variables
// TODO: delete thios once useSail is gone to cpp
unsigned long loggingInterval = 60; // seconds b/w logging events, 1 day =
// 86,400 secs which is max
unsigned int runNum;        // increments each time the device starts
unsigned int loopCount = 0; // increments at each loop
boolean fixAcquired = false, staleFix = true; // for GPS
float batteryVoltage;
boolean batteryCritical = false;
unsigned int timeSinceLastFramLog = 0;
String logSentence = "";
boolean txSuccess;
int orderedSailPosition; // switches b/w A and B

// TODO: delete these once useSail is gone to cpp
int orderedSailPositionA = 270; // where we want the sail to be for A
int orderedTackTimeA = 90;      // how long we want the sail to be in
position A
int orderedSailPositionB = 270; // where we want the sail to be for B
int orderedTackTimeB = 90;      // how long we want the sail to be in
position B

int sailPosition;       // the actual position of the sail
boolean tackIsA = true; // to keep track of which tack setting we should be on
int currentTackTime =
    0; // keeps track of how long we've been on current tack in minutes
unsigned long thisWatch;
boolean rxMessageInvalid = false;
boolean trimRoutineExceededMax =
    false; // if it takes more than set number of pulses to trim the sail
boolean sailNotMoving = false; // if it gets stuck moving sail 1 degree
GbFix fix;
GbSailingOrders sailingOrders = {.loggingInterval = 60,
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

// TODO: figure this out
Sail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
          MOTOR_POWER_ENABLE_PIN, MOTOR_IN_1_PIN, MOTOR_IN_2_PIN);

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
    Serial.println(F("Resetting EEPROM"));
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
      sail.GetPosition(), diagnosticMessage());

  if (USING_WIFI) {
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
    byte wifi_attempt = 1;
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
    }
  } else if (USING_SERIAL_MONITOR_ORDERS) {
    getSerialMonitorOrders();
    txSuccess = true;
  } else {
    getFakeOrders();
    txSuccess = true;
  }

  // Parse inbound message
  if (txSuccess) {
    sailingOrders = GbMessageParser::ParseMessage(inboundMessage);
  }

  // TODO: pass in txSuccess
  thisWatch = howLongWatchShouldBe(); // in seconds

  if (USING_SAIL) {
    sailMode = 'p';
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
    // TODO: move useSail into cpp and pass it the sailing orders
    //useSail();
  } else {
    GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2); // why?
    pretendSail();
  }
}
