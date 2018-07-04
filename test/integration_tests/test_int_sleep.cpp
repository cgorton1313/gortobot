/* This test exercises the sleep functionality */

#include "../../src/configs/config.h"
#include "../../src/configs/consts.h"
#include "../../src/configs/pins.h"
#include "../../src/utilities/gb_utility.h"
#include <Arduino.h>
#include <Sleep_n0m1.h>

Sleep sleeper;

void setup() {

  initPins();

  sleeper.pwrDownMode();

  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN(F("Sleep test starting..."));
}

void loop() {
  DEBUG_PRINTLN("Entering sleep.");
  sleeper.sleepDelay(10 * 1000);
  digitalWrite(LED_PIN, HIGH);
  delay(20);
  digitalWrite(LED_PIN, LOW);
  DEBUG_PRINTLN(F("I am on."));
  delay(2000);
}
