/* This test exercises the message handler */

#include "../../src/communications\gb_message_handler.h"
#include "../../src/communications\gb_sailing_orders.h"
#include <Arduino.h>

void setup() {
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN(F("Message Handler test starting..."));

  // Test happy path
  GbMessageHandler messageHandler = GbMessageHandler();
  bool rxMessageInvalid = false;
  GbTrimResult trimResult = {.success = true,
                             .sailStuck = false,
                             .trimRoutineExceededMax = false,
                             .sailBatteryTooLow = false};

  uint8_t result =
      messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid);

  if (result == 0) {
    DEBUG_PRINTLN(F("Happy Path test - Pass"));
  } else {
    DEBUG_PRINTLN(F("Happy Path test - Fail"));
  }

  // Test rxMessageInvalid
  rxMessageInvalid = true;
  result = messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid);

  if (result == 8) {
    DEBUG_PRINTLN(F("rxInvalid test - Pass"));
  } else {
    DEBUG_PRINTLN(F("rxInvalid test - Fail"));
  }

  // Test mix
  rxMessageInvalid = true;
  trimResult.sailStuck = true;

  result = messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid);

  if (result == 10) {
    DEBUG_PRINTLN(F("Mix test - Pass"));
  } else {
    DEBUG_PRINTLN(F("Mix test - Fail"));
  }
}

// Blink when done
void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }
