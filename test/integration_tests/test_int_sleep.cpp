/* This test exercises thesleep functionality */

#include "configs/pins.h"
#include "utilities/gb_utility.h"
#include <Arduino.h>

void setup() {
  pinMode(WIFI_ENABLE_PIN, OUTPUT);
  digitalWrite(WIFI_ENABLE_PIN, LOW);

  Serial.begin(115200);
  Serial.println(F("Sleep test starting..."));
}

void loop() {
  GbUtility::GortoNap(6);
  delay(100);
}
