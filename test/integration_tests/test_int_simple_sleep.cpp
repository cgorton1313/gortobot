/* This test exercises the sleep functionality */

#include <Arduino.h>
#include <Sleep_n0m1.h>

Sleep sleep;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  sleep.pwrDownMode();
}

void loop() {
  sleep.sleepDelay(6000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(LED_BUILTIN, LOW);
}
