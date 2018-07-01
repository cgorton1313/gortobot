/* This test exercises the Sail object directly */

#include "../../src/configs/consts.h"
#include "../../src/configs/pins.h"
#include "../../src/sailing/gb_sail.h"
#include <Arduino.h>

static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);

int16_t GetSerialMessage() {
  DEBUG_PRINTLN("Ready for orders:");

  // Wait for the serial data
  while (!Serial.available()) {
  }

  return Serial.parseInt();
}

void setup() {
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN(F("Sail integration test starting..."));
}

void loop() {
  DEBUG_PRINT("Sail position is: ");
  DEBUG_PRINTLN(sail.GetSailPosition());

  int16_t orderedSailPosition = GetSerialMessage();
  if (orderedSailPosition >= 0 || orderedSailPosition < 360) {
    DEBUG_PRINT("Trimming to: ");
    DEBUG_PRINTLN(orderedSailPosition);
    sail.Trim(orderedSailPosition);
  } else {
    DEBUG_PRINTLN("Bad input. Skipping.");
  }

  DEBUG_PRINTLN("*******************************************");
  delay(500);
}
