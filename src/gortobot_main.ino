/* Gortobot v2 */

// gortobot_main.ino handles program modes, includes, pin assignments,
//  objects, global constants & variables, setup() & loop()

// TODO:
// nada!

// Program Modes (config)
static const boolean resetEEPROM = false; // sets runNum back to 0
static const boolean resetFRAM = false; // full wipe of FRAM data
static const boolean checkingVoltage = true;
static const boolean usingFram = false;
static const boolean testingFram = false; // prints FRAM contents to serial
static const boolean usingGPS = true;
static const boolean usingWifi = false;
static const boolean usingSat = true;
static const boolean usingSerialMonitorOrders = false;
static const boolean usingSail = true;

// Includes
#include <EEPROM.h> // for saving the runNum after each re-start
#include <Adafruit_FRAM_I2C.h> // for FRAM logging
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <IridiumSBD.h>
#include <Narcoleptic.h> // sleep library, doesn't work with mega

// Pin assignments
// Pro Mini: A4 = SDA (yellow), A5 = SCL (blue), used for FRAM
static const byte ledPin = 13; // brown, to external LED via 420 ohm resistor
static const byte gpsRXpin = 15, gpsTXpin = 16; // 15 (aka A1) blue to the GPS TX, 16 (aka A2) yellow to the GPS RX
static const byte gpsPowerPin = 2; // orange
static const byte batteryVoltagePin = A0; // green
static const byte potPin = A3; // green
static const byte motorIn1Pin = 3, motorIn2Pin = 4; // 3-yellow, 4-blue
static const byte chipSelect = 10; // temp while using only satellite. can't remember why
static const byte satSleepPin = 7, satTXpin = 8, satRXpin = 9; // 7-green, 8-yellow, 9-brown
static const byte wifiTXpin = 5, wifiRXpin = 6; // 5-yellow, don't really need 6

// Constants
static const byte delayForSerial = 5; // ms to delay so serial ouput is clean
static const unsigned long consoleBaud = 115200, wifiBaud = 115200;
static const unsigned int gpsBaud = 9600, satBaud = 19200;
static const int satChargeTime = 30; // seconds to wait at start-up for super-capacitor
static const int isbdTimeout = 600;  // seconds to try getting isbd success
static const byte failureRetry = 10; // minutes to wait after sat failure
static const float minBatteryVoltage = 3.3; // system will wait for charging at this low voltage threshold
static const float batteryOkayVoltage = 3.4; // system will resume program at this voltage threshold
static const int batteryWaitTime = 60; // seconds to wait between checking for batteryOkay
static const unsigned int framLoggingInterval = 60; // in minutes
static const byte messageVersion = 3; // 2 = long form, 3 = base62
static const char base62Chars[63] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const int minSail = 0, maxSail = 360; // limits for sail
static const int trimRoutineMaxSeconds = 900; // max number of trim seconds allowed to get to ordered position. testing shows 450 should be max

// Global variables
unsigned int loggingInterval = 60;  // minutes b/w logging events, 1 day = 1440 mins which is max
unsigned int runNum;  // increments each time the device starts
unsigned int loopCount = 0;  // increments at each loop
boolean fixAcquired = false, staleFix = true;  // for GPS
float latitude, longitude;
unsigned int year;
byte month, day, hour, minute, second;
unsigned int ddmmyy, hhmmss;  // do we need these now that we use base62?
byte satellites;
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
char sailMode = 'r';  // 'r' = real, 't' = test, 's' = set direct. leave as 'r' so on wake up it doesn't go into test
unsigned long thisWatch;
boolean framProblem = false; // if FRAM does begin or loops more the 16 times to find an open memory adress
boolean rxMessageInvalid = false;
boolean trimRoutineExceededMax = false; // if it takes more than set number of pulses to trim the sail
boolean sailNotMoving = false; // if it gets stuck moving sail 1 degree

// Objects
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C(); // onboard data logger
SoftwareSerial gpsSS(gpsRXpin, gpsTXpin);
SoftwareSerial satSS(satRXpin, satTXpin);
SoftwareSerial wifiSS(wifiRXpin, wifiTXpin); // for at-home testing
TinyGPSPlus tinyGPS;
IridiumSBD isbd(satSS, satSleepPin);

void setup() {
        randomSeed(analogRead(A7)); // for faking data differently each run, A7 should be open
        Serial.begin(consoleBaud);

        // Pin Modes
        pinMode(ledPin, OUTPUT);
        pinMode(gpsPowerPin, OUTPUT);
        pinMode(batteryVoltagePin, INPUT); // not necessary but for clarity
        pinMode(satSleepPin, OUTPUT);
        pinMode(motorIn1Pin, OUTPUT);
        pinMode(motorIn2Pin, OUTPUT);
        analogWrite(ledPin, LOW); // turn off LED
        digitalWrite(gpsPowerPin, LOW); // turn off GPS
        isbd.sleep(); // turn off ISBD

        if (resetEEPROM) {
                Serial.println(F("Resetting EEPROM"));
                clearEEPROM();
        }
        incrementRunNum();
        Serial.print(F("Starting runNum "));
        Serial.println(runNum);

        if (resetFRAM) {
                Serial.println(F("Resetting FRAM"));
                clearFRAM();
        }

        if (usingSat) setUpSat();

        delay(4000); // for some forgotten reason
}

void loop() {
        loopCount++; // loop counter
        if (usingGPS) {
                getFix('r'); // 'r' = 'real'
        }
        else {
                getFix('f'); // 'f' = 'fake'
        }
        if (usingFram) useFram();
        logSentence = makeLogSentence();
        if (usingWifi) useWifi();
        if (usingSat) {
                useSat();
        }
        else if (usingSerialMonitorOrders) {
                getSerialMonitorOrders();
        }
        else {
                getFakeOrders();
        }
        thisWatch = howLongWatchShouldBe(); // in seconds
        if (usingSail) {
                useSail();
        }
        else {
                pretendSail();
        }
}
