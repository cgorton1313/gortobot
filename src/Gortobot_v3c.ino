/* Gortobot v3c */

// TODO:
// battery classes, abstract, fake
// gps classes, abstract, fake
// get rid of warnings in build log

// Program Modes (config)
#include "configs/config.h"

// Includes
#include <EEPROM.h> // for saving the runNum after each re-start
#include <Adafruit_FRAM_I2C.h> // for FRAM logging
#include <IridiumSBD.h>
#include <Narcoleptic.h>
#include "communication/gb_wifi.h"
#include "power/gb_battery.h"
//#include "navigation/gb_fix.h" // can't include twice, but is there a better way?
#include "navigation/gb_gps.h"

// Pin assignments
#define GPS_PORT Serial1
#define WIFI_PORT Serial2
#define ISBD_PORT Serial3
const byte LED_PIN = 13; // brown, to external LED via 420 ohm resistor
const byte GPS_POWER_PIN_1 = 26;
const byte GPS_POWER_PIN_2 = 25;
const byte BATTERY_VOLTAGE_PIN = A0; // green
const byte POT_PIN = A3; // green
const byte MOTOR_IN_1_PIN = 3, MOTOR_IN_2_PIN = 4; // 3-yellow, 4-blue
const byte CHIP_SELECT = 10; // temp while using only satellite. can't remember why
const byte SATELLITE_SLEEP_PIN = 7; // 7-green
const byte WIFI_ENABLE_PIN = 4; // pin 3 on Mega Pro is "D4", (brown)
const byte RANDOM_SEED_PIN = A7;

// Constants
const byte DELAY_FOR_SERIAL = 5; // ms to delay so serial ouput is clean
const unsigned long CONSOLE_BAUD = 115200, WIFI_BAUD = 115200, GPS_BAUD = 38400;
const unsigned int SAT_BAUD = 19200;
const int SAT_CHARGE_TIME = 30; // seconds to wait at start-up for super-capacitor
const int ISBD_TIMEOUT = 600;  // seconds to try getting isbd success
const byte FAILURE_RETRY = 10; // minutes to wait after sat failure
const byte WIFI_ATTEMPT_LIMIT = 3; // number of times to try connecting to wifi
const float MINIMUM_BATTERY_VOLTAGE = 3.3; // system will wait for charging at this low voltage threshold
const float BATTERY_OKAY_VOLTAGE = 3.4; // system will resume program at this voltage threshold
const int BATTERY_WAIT_TIME = 60; // seconds to wait between checking for batteryOkay
const unsigned int FRAM_LOGGING_INTERVAL = 60; // in minutes
const byte MESSAGE_VERSION = 3; // 2 = long form, 3 = base62
const char BASE_62_CHARACTERS[63] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const int MINIMUM_SAIL_ANGLE = 0, MAXIMUM_SAIL_ANGLE = 360; // limits for sail
const int TRIM_ROUTINE_MAXIMUM_SECONDS = 900; // max number of trim seconds allowed to get to ordered position. testing shows 450 should be max

// Global variables
unsigned long loggingInterval = 10;  // seconds b/w logging events, 1 day = 86,400 secs which is max
unsigned int runNum;  // increments each time the device starts
unsigned int loopCount = 0;  // increments at each loop
boolean fixAcquired = false, staleFix = true;  // for GPS
float batteryVoltage;
boolean batteryCritical = false;
unsigned int timeSinceLastFramLog = 0;
const byte framElements = 12;  // number of elements captured in each FRAM log
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
boolean framProblem = false; // if FRAM doesn't begin or loops more the 16 times to find an open memory address
boolean rxMessageInvalid = false;
boolean trimRoutineExceededMax = false; // if it takes more than set number of pulses to trim the sail
boolean sailNotMoving = false; // if it gets stuck moving sail 1 degree
GbFix fix;

// Objects
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C(); // onboard data logger
GbGps gb_gps = GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_PORT, GPS_BAUD);
IridiumSBD isbd(ISBD_PORT, SATELLITE_SLEEP_PIN);
GbWifi wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_PORT, WIFI_BAUD);
GbBattery battery = GbBattery(BATTERY_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME, CHECKING_VOLTAGE);

void setup() {
        analogReference(EXTERNAL);
        for (byte i = 0; i < 100; i++) { // clear the bits
                analogRead(A0);
        }

        randomSeed(analogRead(RANDOM_SEED_PIN)); // for faking data differently each run, A7 should be open
        Serial.begin(CONSOLE_BAUD);

        // Pin Modes
        pinMode(LED_PIN, OUTPUT);
        pinMode(GPS_POWER_PIN_1, OUTPUT);
        pinMode(GPS_POWER_PIN_2, OUTPUT);
        pinMode(BATTERY_VOLTAGE_PIN, INPUT); // not necessary but for clarity
        pinMode(SATELLITE_SLEEP_PIN, OUTPUT);
        pinMode(MOTOR_IN_1_PIN, OUTPUT);
        pinMode(MOTOR_IN_2_PIN, OUTPUT);
        analogWrite(LED_PIN, LOW); // turn off LED
        digitalWrite(GPS_POWER_PIN_1, LOW); // turn off GPS
        digitalWrite(GPS_POWER_PIN_2, LOW); // turn off GPS
        isbd.sleep(); // turn off ISBD

        if (RESET_EEPROM) {
                Serial.println(F("Resetting EEPROM"));
                clearEEPROM();
        }
        incrementRunNum();
        Serial.print(F("Starting runNum "));
        Serial.println(runNum);

        if (RESET_FRAM) {
                Serial.println(F("Resetting FRAM"));
                clearFRAM();
        }

        if (USING_SAT) setUpSat();

        delay(4000); // for some forgotten reason
}

void loop() {
        loopCount++; // loop counter
        if (USING_GPS) {
                battery.Okay();
                fix = gb_gps.GetFix('r'); // 'r' = 'real'
        } else {
                battery.Okay();
                fix = gb_gps.GetFix('f'); // 'f' = 'fake'
        }
        if (USING_FRAM) useFram(fix);
        batteryVoltage = battery.GetVoltage();
        logSentence = makeLogSentence(fix);

        if (USING_WIFI) {
                byte wifi_attempt = 1;
                bool wifi_successful = false;
                txSuccess = false;
                while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {
                        if (wifi.UseWifi(logSentence)) {
                            wifi_successful = true;
                            txSuccess = true;
                        }
                        wifi_attempt++;
                }
        }

        if (USING_SAT) {
                useSat();
        } else if (USING_SERIAL_MONITOR_ORDERS) {
                getSerialMonitorOrders();
        } else {
                getFakeOrders();
        }

        thisWatch = howLongWatchShouldBe(); // in seconds
        if (USING_SAIL) {
                useSail();
        } else {
                pretendSail();
        }
}
