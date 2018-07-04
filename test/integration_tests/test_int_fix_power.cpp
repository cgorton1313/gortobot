/* This test exercises the sleep functionality */

#include "../../src/configs/pins.h"
#include <Arduino.h>
#include <Sleep_n0m1.h>

Sleep sleep;

void setup() {
  initPins();
  sleep.pwrDownMode();
}

void loop() {
  sleep.sleepDelay(6000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(LED_BUILTIN, LOW);
}
