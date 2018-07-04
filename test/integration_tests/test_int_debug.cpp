/* This test exercises the debug functionality */

#include "../../src/configs/config.h"
#include <Arduino.h>

void setup() {
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN(F("Debug test starting..."));
}

void loop() {
  DEBUG_PRINT("I am ");
  DEBUG_PRINTLN(" here.");
  delay(500);
}
