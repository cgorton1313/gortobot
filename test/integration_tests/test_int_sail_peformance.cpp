/* This test exercises the sail performance */

// Includes
#include "../../src/configs/config.h"
#include "../../src/configs/includes.h"

// Constants
#include "configs/consts.h"

// Objects
static GbSail sail(SAIL_POSITION_SENSOR_PIN, SAIL_POSITION_ENABLE_PIN,
                   MOTOR_POWER_ENABLE_PIN, MOTOR_DIRECTION_PIN, MOTOR_SPEED_PIN,
                   MIN_SAIL_ANGLE, MAX_SAIL_ANGLE,
                   TRIM_ROUTINE_MAXIMUM_SECONDS, MAST_POSITION_CALIBRATION);

void setup() {
  initPins();

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Sail perf test starting..."));
}

void loop() {
  int angles[] = {0, 90, 120, 150, 180, 210, 240, 270};

  for (int i = 0; i < (sizeof(angles) / sizeof(int)); i++) {
    Serial.println(angles[i]);
    sail.Trim(angles[i]);
    delay(6 * 1000);
  }
}
