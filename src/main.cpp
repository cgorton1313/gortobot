/* Gortobot v3c

The main.cpp program handles the necessary includes, pin assignments, global
consts, etc.
Then, after setup, it runs the loop. The loop simply calls for a position (gps),
sends and receives data, then executes the perceived orders. Rinse and repeat.

*/

// TODO: integrate IridiumSBD 2.0 and test
// TODO: watchdog timer
// TODO: more unit tests
// TODO: use constructor inits

#ifndef UNIT_TEST // required for platformio unit testing

// Includes
#include "configs/includes.h"

// Constants
#include "configs/consts.h"

// Global variables
// TODO: delete this once useSail is gone to cpp
static uint32_t loggingInterval =
    60; // seconds b/w logging events, 1 day = 86,400 secs which is max
static uint16_t runNum;        // increments each time the device starts
static uint16_t loopCount = 0; // increments at each loop
static uint16_t currentTackTime =
    0; // track how long we've been on current tack in minutes
static uint32_t thisWatch;
static bool rxMessageInvalid = false; // TODO: re-implement this
static GbFix fix; // last known position and time of the vessel
static GbSailingOrders sailingOrders = {.sailMode = 'r',
                                        .validOrders = true,
                                        .loggingInterval = 60,
                                        .orderedSailPositionA = 270,
                                        .orderedTackTimeA = 90,
                                        .orderedSailPositionB = 270,
                                        .orderedTackTimeB = 90};

// Objects
static GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);
static GbSatcom gb_satcom =
    GbSatcom(SATELLITE_SLEEP_PIN, SATCOM_SERIAL_PORT, SAT_BAUD);
static GbWifi gb_wifi = GbWifi(WIFI_ENABLE_PIN, WIFI_SERIAL_PORT, WIFI_BAUD);
static GbRealBattery battery1 =
    GbRealBattery(1, BATTERY_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE,
                  BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME);
static GbRealBattery battery2 =
    GbRealBattery(2, BATTERY2_VOLTAGE_PIN, MINIMUM_BATTERY_VOLTAGE,
                  BATTERY_OKAY_VOLTAGE, BATTERY_WAIT_TIME);
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_IN_1_PIN, MOTOR_IN_2_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);
static GbWatchStander watchStander = GbWatchStander(LED_PIN);
static GbMessageHandler messageHandler = GbMessageHandler(MESSAGE_VERSION);

void setup() {
  // for random numbers, A7 should be a floating pin
  randomSeed(analogRead(RANDOM_SEED_PIN));

  Serial.begin(CONSOLE_BAUD);

  // Pin Modes
  // TODO: these are handled by constructors. Is that okay? Check to see if it
  // still works with outputs commented out
  pinMode(LED_PIN, OUTPUT);
  // pinMode(GPS_POWER_PIN_1, OUTPUT);
  // pinMode(GPS_POWER_PIN_2, OUTPUT);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);  // not necessary but for clarity
  pinMode(BATTERY2_VOLTAGE_PIN, INPUT); // not necessary but for clarity
  // pinMode(SATELLITE_SLEEP_PIN, OUTPUT);
  // pinMode(MOTOR_IN_1_PIN, OUTPUT);
  // pinMode(MOTOR_IN_2_PIN, OUTPUT);
  // pinMode(WIFI_ENABLE_PIN, OUTPUT);

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

  // TODO check this
  gb_satcom.SetUpSat(SAT_CHARGE_TIME, ISBD_TIMEOUT);

  delay(1000);
}

void loop() {
  loopCount++;

  // Get a position
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  fix = gb_gps.GetFix();

  // Construct the outbound message as a string
  String logSentence = messageHandler.BuildOutboundMessage(
      runNum, loopCount, fix, battery1.GetVoltage(), battery2.GetVoltage(),
      sail.GetSailPosition(), messageHandler.GetDiagnosticMessage());

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
    Serial.print(F("Received inbound message of: "));
    Serial.println(inboundMessage);
  }

  // Parse inbound message
  if (txSuccess) {
    sailingOrders = messageHandler.ParseMessage(inboundMessage);
    thisWatch = loggingInterval; // TODO deprecate this variable?
  } else {
    thisWatch = min(FAILURE_RETRY, loggingInterval);
  }

  // Execute sailing orders for one logging interval
  sailingOrders.sailMode = sailMode; // for now
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  watchStander.StandWatch(sail, sailingOrders);
}

#endif // for platformio unit testing
