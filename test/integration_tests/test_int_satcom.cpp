/* This test exercises the SatCom */

// Includes
#include "../../src/communications/gb_message_handler.h"
#include "communications/gb_satcom.h"
#include "configs/pins.h"
#include "power/gb_abstract_battery.h"
#include "power/gb_real_battery.h"
#include "utilities/gb_utility.h"
#include <Arduino.h>

// Constants
#include "configs/consts.h"

// Global variables
static uint16_t runNum;        // increments each time the device starts
static uint16_t loopCount = 0; // increments at each loop

// Objects
static GbRealBattery battery1 = GbRealBattery(
    1, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY_VOLTAGE_PIN);
static GbRealBattery battery2 = GbRealBattery(
    2, MINIMUM_BATTERY_VOLTAGE, BATTERY_OKAY_VOLTAGE, BATTERY2_VOLTAGE_PIN);
static GbSatcom gb_satcom =
    GbSatcom(SATELLITE_SLEEP_PIN, SATCOM_SERIAL_PORT, SAT_BAUD);
static GbMessageHandler messageHandler = GbMessageHandler();

void setup() {
  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Satcom test starting..."));

  // Pin Modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  pinMode(BATTERY2_VOLTAGE_PIN, INPUT);
  pinMode(SATELLITE_SLEEP_PIN, OUTPUT);

  // Initial pin states
  analogWrite(LED_PIN, LOW);

  runNum = GbUtility::IncrementRunNum();
  DEBUG_PRINT(F("Starting runNum "));
  DEBUG_PRINTLN(runNum);

  gb_satcom.SetUpSat(SAT_CHARGE_TIME, ISBD_TIMEOUT);
}

void loop() {
  loopCount++;

  // Construct the outbound message as a string with fake values
  // TODO: rx valid
  bool rxMessageInvalid = true;
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
      battery2.GetVoltage(), 180,
      messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid));
  DEBUG_PRINT(F("logSentence = "));
  DEBUG_PRINTLN(logSentence);

  // Send/receive message via satcom
  GbUtility::WaitForBatteries(BATTERY_WAIT_TIME, battery1, battery2);
  String inboundMessage = "";
  bool txSuccess = false;
  if (gb_satcom.UseSatcom(logSentence)) {
    txSuccess = true;
    inboundMessage = gb_satcom.GetInboundMessage();
    DEBUG_PRINT(F("Received inbound message of: "));
    DEBUG_PRINTLN(inboundMessage);
  } else {
    txSuccess = false;
    DEBUG_PRINTLN(F("No message received."));
  }
  DEBUG_PRINTLN(txSuccess);

  // Sleep for some minutes
  GbUtility::GortoNap(1 * 60);
}
