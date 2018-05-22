/* This test exercises the message handler */

#include "communications\gb_message_handler.h"
#include "communications\gb_sailing_orders.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println(F("Message Handler test starting..."));

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
    Serial.println(F("Happy Path test - Pass"));
  } else {
    Serial.println(F("Happy Path test - Fail"));
  }

  // Test rxMessageInvalid
  rxMessageInvalid = true;
  result = messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid);

  if (result == 8) {
    Serial.println(F("rxInvalid test - Pass"));
  } else {
    Serial.println(F("rxInvalid test - Fail"));
  }

  // Test mix
  rxMessageInvalid = true;
  trimResult.sailStuck = true;

  result = messageHandler.GetDiagnosticMessage(trimResult, rxMessageInvalid);

  if (result == 10) {
    Serial.println(F("Mix test - Pass"));
  } else {
    Serial.println(F("Mix test - Fail"));
  }
}

// Blink when done
void loop() { digitalWrite(LED_BUILTIN, (millis() / 500) % 2); }
