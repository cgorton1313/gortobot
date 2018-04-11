/* This test exercises the Sail object */

#include "power/gb_abstract_battery.h"
#include "utilities/gb_utility.h"
#include <Arduino.h>

String GetSerialMessage() {
  Serial.println("Ready for orders:");

  // Wait for the serial data
  while (!Serial.available()) {
  }

  return Serial.readString();
}


void setup() {
  Serial.begin(115200);
  Serial.println(F("Sail integration test starting..."));
}

void loop() {
// implement sail test
  Serial.println("*******************************************");
  delay(500);
}
