/* Gortobot v3c */

// Gortobot_v3c.ino handles program modes, includes, pin assignments,
//  objects, global constants & variables, setup() & loop()

// TODO:
// config switches?
// change isbd to use hardware serial
// battery classes, abstract, fake
// gps classes, abstract, fake

// Program Modes (config)
#include <configs/config.h>
const boolean resetEEPROM = RESETEEPROM; // sets runNum back to 0
const boolean resetFRAM = RESETFRAM; // full wipe of FRAM data
const boolean checkingVoltage = CHECKINGVOLTAGE;
const boolean usingFram = USINGFRAM;
const boolean testingFram = TESTINGFRAM; // prints FRAM contents to serial
const boolean usingGPS = USINGGPS;
const boolean usingWifi = USINGWIFI;
const boolean usingSat = USINGSAT;
const boolean usingSerialMonitorOrders = USINGSERIALMONITORORDERS;
const boolean usingSail = USINGSAIL;

// Includes
#include <EEPROM.h> // for saving the runNum after each re-start
#include <Adafruit_FRAM_I2C.h> // for FRAM logging
#include <NMEAGPS.h>
#include <IridiumSBD.h>
#include <Narcoleptic.h> // sleep library, doesn't work with mega, or does it?
#include <communication/Wifi.h>
#include <power/Battery.h>

// Pin assignments
// Pro Mini: A4 = SDA (yellow), A5 = SCL (blue), used for FRAM
const byte ledPin = 13; // brown, to external LED via 420 ohm resistor
const byte gpsRXpin = 15, gpsTXpin = 16; // 15 (aka A1) blue to the GPS TX, 16 (aka A2) yellow to the GPS RX
const byte gpsPowerPin = 26;
#define gpsPort Serial1
const byte batteryVoltagePin = A0; // green
const byte potPin = A3; // green
const byte motorIn1Pin = 3, motorIn2Pin = 4; // 3-yellow, 4-blue
const byte chipSelect = 10; // temp while using only satellite. can't remember why
const byte satSleepPin = 7; // 7-green
#define wifiPort Serial2
#define isbdPort Serial3
const byte wifiEnablePin = 4; // pin 3 on Mega Pro is "D4", (brown)

// Constants
const byte delayForSerial = 5; // ms to delay so serial ouput is clean
const unsigned long consoleBaud = 115200, wifiBaud = 115200, gpsBaud = 38400;
const unsigned int satBaud = 19200;
const int satChargeTime = 30; // seconds to wait at start-up for super-capacitor
const int isbdTimeout = 600;  // seconds to try getting isbd success
const byte failureRetry = 10; // minutes to wait after sat failure
const float minBatteryVoltage = 3.3; // system will wait for charging at this low voltage threshold
const float batteryOkayVoltage = 3.4; // system will resume program at this voltage threshold
const int batteryWaitTime = 60; // seconds to wait between checking for batteryOkay
const unsigned int framLoggingInterval = 60; // in minutes
const byte messageVersion = 2; // 2 = long form, 3 = base62
const char base62Chars[63] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const int minSail = 0, maxSail = 360; // limits for sail
const int trimRoutineMaxSeconds = 900; // max number of trim seconds allowed to get to ordered position. testing shows 450 should be max

// Global variables
unsigned long loggingInterval = 8;  // seconds b/w logging events, 1 day = 86,400 secs which is max
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
gps_fix fix;
bool fixDone = false;

// Objects
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C(); // onboard data logger
NMEAGPS gps;
IridiumSBD isbd(isbdPort, satSleepPin);
Wifi wifi = Wifi(wifiEnablePin, wifiPort);
Battery battery = Battery(batteryVoltagePin, minBatteryVoltage, batteryOkayVoltage, batteryWaitTime, checkingVoltage);

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
                battery.okay();
                getFix('r'); // 'r' = 'real'
        }
        else {
                battery.okay();
                getFix('f'); // 'f' = 'fake'
        }
        if (usingFram) useFram();
        batteryVoltage = battery.getVoltage();
        logSentence = makeLogSentence();
        if (usingWifi) wifi.useWifi(logSentence);
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
