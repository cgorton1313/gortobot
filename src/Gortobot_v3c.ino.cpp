# 1 "c:\\users\\craig\\appdata\\local\\temp\\tmp4ympou"
#include <Arduino.h>
# 1 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/Gortobot_v3c.ino"
# 17 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/Gortobot_v3c.ino"
#include "configs/config.h"





#include <EEPROM.h>

#include <IridiumSBD.h>

#include <Sleep_n0m1.h>

#include "communication/gb_wifi.h"

#include "power/gb_battery.h"

#include "navigation/gb_gps.h"

#include "communication/gb_sentence_builder.h"





#define GPS_PORT Serial1

#define WIFI_PORT Serial2

#define ISBD_PORT Serial3

const byte LED_PIN = 13;

const byte GPS_POWER_PIN_1 = 26;

const byte GPS_POWER_PIN_2 = 25;

const byte BATTERY_VOLTAGE_PIN = A0;

const byte POT_PIN = A3;

const byte MOTOR_IN_1_PIN = 3, MOTOR_IN_2_PIN = 4;

const byte CHIP_SELECT = 10;

const byte SATELLITE_SLEEP_PIN = 7;

const byte WIFI_ENABLE_PIN = 4;

const byte RANDOM_SEED_PIN = A7;





const byte DELAY_FOR_SERIAL = 5;

const unsigned long CONSOLE_BAUD = 115200, WIFI_BAUD = 115200, GPS_BAUD = 38400;

const unsigned int SAT_BAUD = 19200;

const int SAT_CHARGE_TIME = 30;

const int ISBD_TIMEOUT = 600;

const byte FAILURE_RETRY = 600;

const byte WIFI_ATTEMPT_LIMIT = 3;

const float MINIMUM_BATTERY_VOLTAGE = 3.3;

const float BATTERY_OKAY_VOLTAGE = 3.4;

const int BATTERY_WAIT_TIME = 60;

const byte MESSAGE_VERSION = 3;

const int MINIMUM_SAIL_ANGLE = 0, MAXIMUM_SAIL_ANGLE = 360;

const int TRIM_ROUTINE_MAXIMUM_SECONDS = 900;





unsigned long loggingInterval = 6;

unsigned int runNum;

unsigned int loopCount = 0;

boolean fixAcquired = false, staleFix = true;

float batteryVoltage;

boolean batteryCritical = false;

unsigned int timeSinceLastFramLog = 0;

String logSentence = "";

char txBuffer[50];

boolean txSuccess;

int orderedSailPosition;

int orderedSailPositionA = 270;

int orderedTackTimeA = 90;

int orderedSailPositionB = 270;

int orderedTackTimeB = 90;

int sailPosition;

boolean tackIsA = true;

int currentTackTime = 0;

unsigned long thisWatch;

boolean rxMessageInvalid = false;

boolean trimRoutineExceededMax = false;

boolean sailNotMoving = false;

GbFix fix;





GbGps gb_gps = GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_PORT, GPS_BAUD);

IridiumSBD isbd(ISBD_PORT, SATELLITE_SLEEP_PIN);

GbWifi wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_PORT, WIFI_BAUD);

GbBattery battery = GbBattery(BATTERY_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME, CHECKING_VOLTAGE);

GbSentenceBuilder sentence_builder = GbSentenceBuilder(MESSAGE_VERSION);

Sleep sleep;
void setup();
void loop();
void parseRxBuffer(String rxBufferAsString);
void blinkMessage(int condition);
void getSerialMonitorOrders();
void getFakeOrders();
byte diagnosticMessage();
static void gortoNap(int seconds);
static void processVoltageData();
static void getVolts();
int getSailPosition();
unsigned long howLongWatchShouldBe();
static void useSail();
static void realSail();
static void testSail();
static void directSetSail();
static boolean validOrders(int order);
static void trimSail(int orderedSailPosition);
static void stopMotors();
static void setTestSailPosition(int thePosition);
static void pretendSail();
static void setUpSat();
static void useSat();
static void satOff();
static void satOn();
static void clearEEPROM();
static void incrementRunNum();
static void EEPROMWritelong(int address, long value);
static long EEPROMReadlong(long address);
#line 165 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/Gortobot_v3c.ino"
void setup() {

        randomSeed(analogRead(RANDOM_SEED_PIN));

        Serial.begin(CONSOLE_BAUD);





        pinMode(LED_PIN, OUTPUT);

        pinMode(GPS_POWER_PIN_1, OUTPUT);

        pinMode(GPS_POWER_PIN_2, OUTPUT);

        pinMode(BATTERY_VOLTAGE_PIN, INPUT);

        pinMode(SATELLITE_SLEEP_PIN, OUTPUT);

        pinMode(MOTOR_IN_1_PIN, OUTPUT);

        pinMode(MOTOR_IN_2_PIN, OUTPUT);

        analogWrite(LED_PIN, LOW);

        digitalWrite(GPS_POWER_PIN_1, LOW);

        digitalWrite(GPS_POWER_PIN_2, LOW);

        digitalWrite(WIFI_ENABLE_PIN, LOW);



        isbd.sleep();



        sleep.pwrDownMode();



        if (RESET_EEPROM) {

                Serial.println(F("Resetting EEPROM"));

                clearEEPROM();

        }

        incrementRunNum();

        Serial.print(F("Starting runNum "));

        Serial.println(runNum);



        if (USING_SAT) setUpSat();



        delay(4000);

}



void loop() {

        loopCount++;



        battery.Okay();

        if (USING_GPS) {

                fix = gb_gps.GetFix('r');

        }

        else {

                fix = gb_gps.GetFix('f');

        }



        logSentence = sentence_builder.Sentence(runNum, loopCount, fix, battery.GetVoltage(),

                                                getSailPosition(), diagnosticMessage());



        battery.Okay();

        if (USING_WIFI) {

                byte wifi_attempt = 1;

                bool wifi_successful = false;

                while (wifi_attempt <= WIFI_ATTEMPT_LIMIT && !wifi_successful) {

                        if (wifi.UseWifi(logSentence)) {

                                wifi_successful = true;

                        }

                        wifi_attempt++;

                }

        }



        battery.Okay();

        if (USING_SAT) {

                useSat();

        }

        else if (USING_SERIAL_MONITOR_ORDERS) {

                getSerialMonitorOrders();

        }

        else {

                getFakeOrders();

        }





        txSuccess = true;

        thisWatch = howLongWatchShouldBe();



        battery.Okay();

        if (USING_SAIL) {

                useSail();

        }

        else {

                pretendSail();

        }

}
# 1 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/communication.ino"
# 15 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/communication.ino"
void parseRxBuffer(String rxBufferAsString) {

        rxMessageInvalid = false;

        if (rxBufferAsString.indexOf("z") >= 0) {

                switch ((rxBufferAsString.substring(0, rxBufferAsString.indexOf(','))).toInt()) {

                case 1: {

                        sailMode = 'r';



                        byte firstCommaIndex = rxBufferAsString.indexOf(',');

                        byte secondCommaIndex = rxBufferAsString.indexOf(',', firstCommaIndex + 1);

                        byte thirdCommaIndex = rxBufferAsString.indexOf(',', secondCommaIndex + 1);

                        byte fourthCommaIndex = rxBufferAsString.indexOf(',', thirdCommaIndex + 1);

                        byte fifthCommaIndex = rxBufferAsString.indexOf(',', fourthCommaIndex + 1);

                        byte sixthCommaIndex = rxBufferAsString.indexOf(',', fifthCommaIndex + 1);

                        String firstValue = rxBufferAsString.substring(0, firstCommaIndex);

                        String secondValue = rxBufferAsString.substring(firstCommaIndex + 1, secondCommaIndex);

                        String thirdValue = rxBufferAsString.substring(secondCommaIndex + 1, thirdCommaIndex);

                        String fourthValue = rxBufferAsString.substring(thirdCommaIndex + 1, fourthCommaIndex);

                        String fifthValue = rxBufferAsString.substring(fourthCommaIndex + 1, fifthCommaIndex);

                        String sixthValue = rxBufferAsString.substring(fifthCommaIndex + 1, sixthCommaIndex);



                        orderedSailPositionA = secondValue.toInt();

                        orderedTackTimeA = thirdValue.toInt();

                        orderedSailPositionB = fourthValue.toInt();

                        orderedTackTimeB = fifthValue.toInt();

                        unsigned long tempLoggingInterval = sixthValue.toInt();

                        if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) {

                                loggingInterval = tempLoggingInterval;

                        }

                        break;

                }

                case 2: {

                        sailMode = 't';

                        orderedSailPositionA = 180;

                        orderedTackTimeA = 1;

                        orderedSailPositionA = 180;

                        orderedTackTimeB = 1;

                        byte firstCommaIndex = rxBufferAsString.indexOf(',');

                        byte secondCommaIndex = rxBufferAsString.indexOf(',', firstCommaIndex + 1);

                        String secondValue = rxBufferAsString.substring(firstCommaIndex + 1, secondCommaIndex);

                        unsigned long tempLoggingInterval = secondValue.toInt();

                        if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) {

                                loggingInterval = tempLoggingInterval;

                        }

                        break;

                }

                case 3: {

                        sailMode = 's';

                        byte firstCommaIndex = rxBufferAsString.indexOf(',');

                        byte secondCommaIndex = rxBufferAsString.indexOf(',', firstCommaIndex + 1);

                        String secondValue = rxBufferAsString.substring(firstCommaIndex + 1, secondCommaIndex);

                        orderedSailPosition = secondValue.toInt();

                        loggingInterval = 1;

                        break;

                }

                default: {

                        Serial.println(F("Not a valid rx message type."));

                }

                }

        }

        else {

                rxMessageInvalid = true;

                Serial.println(F("No valid message (no z in message)"));

        }



        Serial.print(F("Logging interval = "));

        Serial.println(loggingInterval);

        Serial.print(F("Sail order A = "));

        Serial.print(orderedSailPositionA);

        Serial.print(F(" for "));

        Serial.print(orderedTackTimeA);

        Serial.println(F(" minutes"));

        Serial.print(F("Sail order B = "));

        Serial.print(orderedSailPositionB);

        Serial.print(F(" for "));

        Serial.print(orderedTackTimeB);

        Serial.println(F(" minutes"));

}





void blinkMessage(int condition) {

        switch (condition) {

        case 1:

                while (true) {

                        digitalWrite(LED_PIN, (millis() / 100) % 2 == 1 ? HIGH : LOW);

                }

                break;

        case 2:

                digitalWrite(LED_PIN, HIGH);

                delay(DELAY_FOR_SERIAL);

                digitalWrite(LED_PIN, LOW);

                break;

        case 3:

                while (true) {

                        digitalWrite(LED_PIN, HIGH);

                        delay(2000);

                        digitalWrite(LED_PIN, LOW);

                        delay(500);

                }

                break;

        }

}



void getSerialMonitorOrders() {

        getSailPosition();

        Serial.println(F("Ready for orders:"));

        while (!Serial.available()) ;





        String sentence = Serial.readString();

        size_t rxBufferSize = sentence.length() + 1;

        char rxBuffer[rxBufferSize];

        sentence.toCharArray(rxBuffer, rxBufferSize);

        String newString = (rxBuffer);

        parseRxBuffer(newString);

}



void getFakeOrders() {



        if (sailMode == 't') {

                String fakeOrderString = "3,";

                fakeOrderString += loggingInterval;

                fakeOrderString += ",z";

        }

        else {

                String fakeOrderString = "1,0,1,360,1,";

                fakeOrderString += loggingInterval;

                fakeOrderString += ",z";

                parseRxBuffer(fakeOrderString);

        }

}



byte diagnosticMessage() {
# 289 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/communication.ino"
        return (trimRoutineExceededMax*pow(2,0) + sailNotMoving*pow(2,1) + rxMessageInvalid*pow(2,3));

}
# 1 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/power.ino"



static void gortoNap(int seconds) {
        for (int i = 0; i < seconds; i++) {
                sleep.sleepDelay(1000);
                delay(DELAY_FOR_SERIAL);
        }
}

static void processVoltageData() {
        if (!CHECKING_VOLTAGE) {
                Serial.println(F("Skipping voltage check. Faking voltage at 3.99."));
                batteryVoltage = 3.99;
        }
        else {
                getVolts();
                if (batteryVoltage < MINIMUM_BATTERY_VOLTAGE) {
                        batteryCritical = true;
                }
                while (batteryCritical) {
                        Serial.print(F("Battery critical! Volts = "));
                        Serial.print(batteryVoltage);
                        Serial.print(F(". Waiting "));
                        Serial.print(BATTERY_WAIT_TIME);
                        Serial.println(F(" seconds."));
                        delay(DELAY_FOR_SERIAL);
                        gortoNap(BATTERY_WAIT_TIME);
                        getVolts();
                        if (batteryVoltage > BATTERY_OKAY_VOLTAGE) {
                                batteryCritical = false;
                        }
                }
                Serial.print(F("Battery okay. Volts = "));
                Serial.println(batteryVoltage);
                delay(DELAY_FOR_SERIAL);
        }
}

static void getVolts() {
        int batteryVoltageInt = 0;
        float correction = 1;
        const int samples = 10;
        for (int i = 0; i < samples; i++) {
                delay(5);
                batteryVoltageInt = batteryVoltageInt + analogRead(BATTERY_VOLTAGE_PIN);
        }
        batteryVoltage = 5.08 * correction * (((float)batteryVoltageInt / samples) / 1023.0);
}
# 1 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/sail.ino"




int getSailPosition() {

        int tempSailPosition;

        const byte REPETITIONS = 10;

        int sum = 0;

        for (byte i = 0; i < REPETITIONS; i++) {

                sum = sum + analogRead(POT_PIN);

                delay(1);

        }

        tempSailPosition = map(sum / REPETITIONS, 1023, 15, MINIMUM_SAIL_ANGLE, MAXIMUM_SAIL_ANGLE);

        Serial.print(F("Sail position = "));

        Serial.print(tempSailPosition);

        Serial.print(F(" ("));

        Serial.print(sum / REPETITIONS);

        Serial.print(F(") | Ordered = "));

        Serial.println(orderedSailPosition);

        delay(DELAY_FOR_SERIAL);

        return tempSailPosition;

}



unsigned long howLongWatchShouldBe() {

        if (txSuccess) {

                return loggingInterval;

        }

        else {

                return min(FAILURE_RETRY, loggingInterval);

        }

}



static void useSail() {

        switch (sailMode) {

        case 'r':

                realSail();

                break;

        case 't':

                Serial.println(F("Entering sail test mode."));

                testSail();

                break;

        case 's':

                Serial.println(F("Entering direct set sail mode."));

                directSetSail();

                break;

        }

}



static void realSail() {

        Serial.print(F("Sailing for "));

        Serial.print(thisWatch);

        Serial.println(F(" seconds."));

        delay(DELAY_FOR_SERIAL);

        unsigned long timer = 0;

        while (timer < thisWatch) {

                if ((timer % 60) == 0) {

                        Serial.println(F("At :00 seconds now"));

                        if ((tackIsA && currentTackTime >= orderedTackTimeA) || (!tackIsA && currentTackTime >= orderedTackTimeB)) {

                                tackIsA = !tackIsA;

                                currentTackTime = 0;

                                Serial.println(F("Changing tacks"));

                        }

                        if (tackIsA) {

                                orderedSailPosition = orderedSailPositionA;

                                Serial.println(F("Sailing on tack A"));

                        } else {

                                orderedSailPosition = orderedSailPositionB;

                                Serial.println(F("Sailing on tack B"));

                        }

                        if (validOrders(orderedSailPosition)) {

                                trimSail(orderedSailPosition);

                        }

                        currentTackTime++;

                        Serial.print(F("Current tack time = "));

                        Serial.println(currentTackTime);

                        delay(DELAY_FOR_SERIAL);

                }

                gortoNap(6);

                timer = timer + 6;

                blinkMessage(2);

                delay(DELAY_FOR_SERIAL);

                Serial.print(F("timer = "));

                Serial.println(timer);

                delay(DELAY_FOR_SERIAL);

        }

}



static void testSail() {



        int testSailPositions[] = {0,30,60,90,120,150,180,210,240,270,300,330,360};

        Serial.print(F("Testing sail."));

        for (byte i = 0; i < (sizeof(testSailPositions) / sizeof(int)); i++) {

                orderedSailPosition = testSailPositions[i];

                setTestSailPosition(orderedSailPosition);

        }

}



static void directSetSail() {

        setTestSailPosition(orderedSailPosition);

}



static boolean validOrders(int order) {

        if ((order > MAXIMUM_SAIL_ANGLE) || (order < MINIMUM_SAIL_ANGLE)) {

                return false;

        }

        else {

                return true;

        }

}



static void trimSail(int orderedSailPosition) {

        unsigned long trimTimer;

        int trimSeconds = 0;

        int totalTrimSeconds = 0;

        boolean sailIsTrimming = true;

        processVoltageData();

        sailPosition = getSailPosition();

        int tempSailPosition = sailPosition;

        trimRoutineExceededMax = false;

        sailNotMoving = false;



        if ((abs(sailPosition - orderedSailPosition)) > 4) {

                trimTimer = millis();

                while (((sailPosition - orderedSailPosition) != 0) && sailIsTrimming) {



                        digitalWrite(MOTOR_IN_1_PIN, sailPosition > orderedSailPosition);

                        digitalWrite(MOTOR_IN_2_PIN, sailPosition < orderedSailPosition);

                        if ((unsigned long)(millis() - trimTimer) >= 1000) {

                                trimTimer = millis();

                                totalTrimSeconds = totalTrimSeconds + 1;

                                if (totalTrimSeconds > TRIM_ROUTINE_MAXIMUM_SECONDS) {

                                        sailIsTrimming = false;

                                        trimRoutineExceededMax = true;

                                        Serial.println(F("trimRoutineExceededMax"));

                                }

                                sailPosition = getSailPosition();

                                if (sailPosition == tempSailPosition) {



                                        trimSeconds = trimSeconds + 1;

                                        if (trimSeconds > 10) {



                                                sailIsTrimming = false;

                                                sailNotMoving = true;

                                                Serial.println(F("Sail not moving"));

                                        }

                                }

                                else {

                                        tempSailPosition = sailPosition;

                                        trimSeconds = 0;

                                        sailNotMoving = false;

                                }

                        }

                }

                stopMotors();

        }

        Serial.print(F("totalTrimSeconds = "));

        Serial.println(totalTrimSeconds);

        delay(DELAY_FOR_SERIAL);

}



static void stopMotors() {

        digitalWrite(MOTOR_IN_1_PIN, LOW);

        digitalWrite(MOTOR_IN_2_PIN, LOW);

}



static void setTestSailPosition(int thePosition) {

        trimSail(thePosition);

        unsigned long testTimer = 0;

        if (sailMode != 's') {

                while (testTimer < loggingInterval) {

                        gortoNap(1);

                        delay(DELAY_FOR_SERIAL);

                        testTimer = testTimer + 1;

                        blinkMessage(2);

                        Serial.print(F("timer = "));

                        Serial.println(testTimer);

                        delay(DELAY_FOR_SERIAL);

                }

        }

}



static void pretendSail() {

        delay(DELAY_FOR_SERIAL);

        Serial.print(F("Pretending to sail for "));

        Serial.print(thisWatch);

        Serial.println(F(" seconds."));

        delay(DELAY_FOR_SERIAL);

        unsigned long timer = 0;

        while (timer < thisWatch) {

                gortoNap(1);

                timer = timer + 1;

                blinkMessage(2);

                delay(DELAY_FOR_SERIAL);

                Serial.print(F("timer = "));

                delay(DELAY_FOR_SERIAL);

                Serial.println(timer);

                delay(10);

        }

}
# 1 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/satcom.ino"
static void setUpSat() {
        Serial.print(F("Charging super-capacitor. Waiting "));
        Serial.print(SAT_CHARGE_TIME);
        Serial.println(F(" seconds..."));
        delay(DELAY_FOR_SERIAL);
        gortoNap(SAT_CHARGE_TIME);
        isbd.attachConsole(Serial);
        isbd.setPowerProfile(1);
        isbd.adjustSendReceiveTimeout(ISBD_TIMEOUT);
        isbd.setMinimumSignalQuality(3);
        isbd.useMSSTMWorkaround(false);
}

static void useSat() {
        satOn();
        do {
                char rxBuffer[27];





                size_t rxBufferSize = sizeof(rxBuffer);
                int error = isbd.sendReceiveSBDText(txBuffer, (uint8_t *)rxBuffer, rxBufferSize);
                if (error != 0) {
                        Serial.print(F("sendReceiveSBDText failed: error "));
                        Serial.println(error);
                        txSuccess = false;
                        satOff();
                        return;
                }
                txSuccess = true;
                Serial.print(F("rxBufferSize = "));
                Serial.println(rxBufferSize);
                if (rxBufferSize == 0) {
                        Serial.println(F("No message received"));
                }
                else {
                        Serial.print(F("Message received: "));
                        Serial.println(rxBuffer);
                        parseRxBuffer(rxBuffer);
                        Serial.print(F("Messages left: "));
                        Serial.println(isbd.getWaitingMessageCount());
                }
        } while (isbd.getWaitingMessageCount() > 0);
        satOff();
}

static void satOff() {
        isbd.sleep();
        ISBD_PORT.end();
        blinkMessage(2);
        Serial.println(F("Sat off."));
}

static void satOn() {
        Serial.println(F("Sat on."));
        ISBD_PORT.begin(SAT_BAUD);
        isbd.begin();
}
# 1 "C:/Users/Craig/Documents/Gortobot/three/software/Gortobot_v3c/src/utility.ino"






static void clearEEPROM() {

        for (unsigned int i = 0; i < EEPROM.length(); i++) {

                EEPROM.write(i, 0);

        }

}



static void incrementRunNum() {

        runNum = EEPROMReadlong(0) + 1;

        EEPROMWritelong(0, runNum);

}



static void EEPROMWritelong(int address, long value)

{





        byte four = (value & 0xFF);

        byte three = ((value >> 8) & 0xFF);

        byte two = ((value >> 16) & 0xFF);

        byte one = ((value >> 24) & 0xFF);





        EEPROM.write(address, four);

        EEPROM.write(address + 1, three);

        EEPROM.write(address + 2, two);

        EEPROM.write(address + 3, one);

}



static long EEPROMReadlong(long address)

{



        long four = EEPROM.read(address);

        long three = EEPROM.read(address + 1);

        long two = EEPROM.read(address + 2);

        long one = EEPROM.read(address + 3);





        return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);

}