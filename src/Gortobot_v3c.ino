/* Gortobot v3c */

// TODO: battery classes, abstract, fake
// TODO: gps classes, abstract, fake
// TODO: figure out how to fail from one batt to the next
// TODO: integrate IridiumSBD 2.0 and test
// TODO: config whether to send to serial (debug mode)

// Program Modes (config)
#include "configs/config.h"

// Includes
#include <EEPROM.h> // for saving the runNum after each re-start
#include <IridiumSBD.h>
#include <Sleep_n0m1.h>
#include <gb_library.h>

// Pin assignments
#define GPS_PORT Serial1
#define WIFI_PORT Serial2
#define ISBD_PORT Serial3
const byte LED_PIN = 13; // brown, to external LED via 420 ohm resistor
const byte GPS_POWER_PIN_1 = 26;
const byte GPS_POWER_PIN_2 = 25;
const byte BATTERY_VOLTAGE_PIN = A0; // green
const byte BATTERY2_VOLTAGE_PIN = A2; // green
const byte SAIL_POSITION_SENSOR_PIN = A5; // green
const byte SAIL_POSITION_ENABLE_PIN = 50; // red
// TODO: wire pin 32 up
const byte MOTOR_POWER_ENABLE_PIN = 32;
const byte MOTOR_IN_1_PIN = 30, MOTOR_IN_2_PIN = 31; // on a JST under the board
const byte CHIP_SELECT = 10; // temp while using only satellite. can't remember why
const byte SATELLITE_SLEEP_PIN = 7; // 7-green
const byte WIFI_ENABLE_PIN = 4; // brown
const byte RANDOM_SEED_PIN = A7;

// Constants
const byte DELAY_FOR_SERIAL = 5; // ms to delay so serial ouput is clean
const unsigned long CONSOLE_BAUD = 115200, WIFI_BAUD = 115200, GPS_BAUD = 38400;
const unsigned int SAT_BAUD = 19200;
const int SAT_CHARGE_TIME = 30; // seconds to wait at start-up for super-capacitor
const int ISBD_TIMEOUT = 600;  // seconds to try getting isbd success
const byte FAILURE_RETRY = 600; // seconds to wait after tx failure
const byte WIFI_ATTEMPT_LIMIT = 3; // number of times to try connecting to wifi
const float MINIMUM_BATTERY_VOLTAGE = 3.3; // system will wait for charging at this low voltage threshold
const float BATTERY_OKAY_VOLTAGE = 3.4; // system will resume program at this voltage threshold
const int BATTERY_WAIT_TIME = 2; // seconds to wait between checking for batteryOkay
const byte MESSAGE_VERSION = 4; // 2 = long form, 3 = base62, 4 = base62 and 2 batteries
const int MINIMUM_SAIL_ANGLE = 0, MAXIMUM_SAIL_ANGLE = 360; // limits for sail
const int TRIM_ROUTINE_MAXIMUM_SECONDS = 900; // max number of trim seconds allowed to get to ordered position. testing shows 450 should be max

// Global variables
unsigned long loggingInterval = 1;  // seconds b/w logging events, 1 day = 86,400 secs which is max
unsigned int runNum;  // increments each time the device starts
unsigned int loopCount = 0;  // increments at each loop
boolean fixAcquired = false, staleFix = true;  // for GPS
float batteryVoltage;
boolean batteryCritical = false;
unsigned int timeSinceLastFramLog = 0;
String logSentence = "";
char txBuffer[50];
boolean txSuccess;
int orderedSailPosition;  // switches b/w A and B
int orderedSailPositionA = 270;  // where we want the sail to be for A
int orderedTackTimeA = 90;  // how long we want the sail to be in position A
int orderedSailPositionB = 270;  // where we want the sail to be for B
int orderedTackTimeB = 90;  // how long we want the sail to be in position B
int sailPosition;  // the actual position of the sail
boolean tackIsA = true;  // to keep track of which tack setting we should be on
int currentTackTime = 0;  // keeps track of how long we've been on current tack in minutes
unsigned long thisWatch;
boolean rxMessageInvalid = false;
boolean trimRoutineExceededMax = false; // if it takes more than set number of pulses to trim the sail
boolean sailNotMoving = false; // if it gets stuck moving sail 1 degree
GbFix fix;

// Objects
GbGps gb_gps = GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_PORT, GPS_BAUD);
IridiumSBD isbd(ISBD_PORT, SATELLITE_SLEEP_PIN);
GbWifi wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_PORT, WIFI_BAUD);
GbBattery battery1 = GbBattery(1, BATTERY_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME, CHECKING_VOLTAGE);
GbBattery battery2 = GbBattery(2, BATTERY2_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME, CHECKING_VOLTAGE);
GbSentenceBuilder sentence_builder = GbSentenceBuilder(MESSAGE_VERSION);
Sleep sleep;

// TODO: figure this out
Sail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
          MOTOR_POWER_ENABLE_PIN, MOTOR_IN_1_PIN, MOTOR_IN_2_PIN);

void setup() {
        randomSeed(analogRead(RANDOM_SEED_PIN)); // for faking data differently each run, A7 should be floating
        Serial.begin(CONSOLE_BAUD);

        // Pin Modes
        pinMode(LED_PIN, OUTPUT);
        pinMode(GPS_POWER_PIN_1, OUTPUT);
        pinMode(GPS_POWER_PIN_2, OUTPUT);
        pinMode(BATTERY_VOLTAGE_PIN, INPUT); // not necessary but for clarity
        pinMode(BATTERY2_VOLTAGE_PIN, INPUT); // not necessary but for clarity
        pinMode(SATELLITE_SLEEP_PIN, OUTPUT);
        pinMode(MOTOR_IN_1_PIN, OUTPUT);
        pinMode(MOTOR_IN_2_PIN, OUTPUT);
        pinMode(WIFI_ENABLE_PIN, OUTPUT);
        analogWrite(LED_PIN, LOW); // turn off LED
        digitalWrite(GPS_POWER_PIN_1, LOW); // turn off GPS
        digitalWrite(GPS_POWER_PIN_2, LOW); // turn off GPS
        digitalWrite(WIFI_ENABLE_PIN, LOW); // turn off wifi

        isbd.sleep(); // turn off ISBD

        sleep.pwrDownMode(); // best power saving mode for sleeping

        if (RESET_EEPROM) {
                Serial.println(F("Resetting EEPROM"));
                clearEEPROM();
        }

        incrementRunNum();
        Serial.print(F("Starting runNum "));
        Serial.println(runNum);

        if (USING_SAT) setUpSat();

        delay(2000); // for some forgotten reason
}

void loop() {
        loopCount++;

        // TODO: test this
        if (USING_GPS) {
                waitForBatteries(BATTERY_WAIT_TIME);
                fix = gb_gps.GetFix('r'); // 'r' = 'real'
        }
        else {
                fix = gb_gps.GetFix('f'); // 'f' = 'fake'
        }

        logSentence = sentence_builder.Sentence(runNum, loopCount, fix, battery1.GetVoltage(),
                                                battery2.GetVoltage(), sail.GetPosition(), diagnosticMessage());

        if (USING_WIFI) {
                waitForBatteries(BATTERY_WAIT_TIME);
                byte wifi_attempt = 1;
                bool wifi_successful = false;
                while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
                        if (wifi.UseWifi(logSentence)) {
                                wifi_successful = true;
                        }
                        wifi_attempt++;
                }
        }

        if (USING_SAT) {
                waitForBatteries(BATTERY_WAIT_TIME);
                useSat();
        }
        else if (USING_SERIAL_MONITOR_ORDERS) {
                getSerialMonitorOrders();
        }
        else {
                getFakeOrders();
        }

        // check the txSuccess use here, can't remember if it's correct
        txSuccess = true;
        thisWatch = howLongWatchShouldBe(); // in seconds

        if (USING_SAIL) {
                waitForBatteries(BATTERY_WAIT_TIME);
                // TODO: make the main program handle tacking, all the sail does is trim
                // and wait for batteries?
                //useSail();
        }
        else {
                pretendSail();
        }
}
