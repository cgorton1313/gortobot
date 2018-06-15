/* This test exercises the Sail object directly */

#include "configs/consts.h"
#include "configs/pins.h"
#include "sailing/gb_sail.h"
#include <Arduino.h>

static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS);

int16_t GetSerialMessage() {
  Serial.println("Ready for orders:");

  // Wait for the serial data
  while (!Serial.available()) {
  }

  return Serial.parseInt();
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Sail integration test starting..."));
}

void loop() {
  Serial.print("Sail position is: ");
  Serial.println(sail.GetSailPosition());

  int16_t orderedSailPosition = GetSerialMessage();
  if (orderedSailPosition >= 0 || orderedSailPosition < 360) {
    Serial.print("Trimming to: ");
    Serial.println(orderedSailPosition);
    sail.Trim(orderedSailPosition);
  } else {
    Serial.println("Bad input. Skipping.");
  }

  Serial.println("*******************************************");
  delay(500);
}
