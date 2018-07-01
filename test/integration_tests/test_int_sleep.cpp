/* This test exercises the sleep functionality */

#include "../../src/configs/pins.h"
#include "../../src/configs/consts.h"
#include "../../src/utilities/gb_utility.h"
#include <Arduino.h>

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Sleep test starting..."));
}

void loop() {
  GbUtility::GortoNap(60);
}
