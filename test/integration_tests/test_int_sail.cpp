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

// Use TestSail to cycle thru sail positions per loggingInterval
// void GbWatchStander::TestSail(GbSail sail, GbSailingOrders sailingOrders) {
//   uint16_t testSailPositions[] = {0,   30,  60,  90,  120, 150, 180,
//                              210, 240, 270, 300, 330, 360};
//   for (uint8_t i = 0; i < (sizeof(testSailPositions) / sizeof(int)); i++) {
//     sail.Trim(testSailPositions[i]);
//     unsigned long testTimer = 0;
//     while (testTimer <
//            sailingOrders.loggingInterval) { // so the duration can be set via RX
//       GbUtility::GortoNap(1);               // one second of napping
//       testTimer = testTimer + 1;
//       // blinkMessage(2); // flash led
//       Serial.print(F("timer = "));
//       Serial.println(testTimer);
//     }
//   }
// }

// void GbWatchStander::FakeSail(unsigned long watchDuration) {
//   Serial.print(F("Fake sailing for "));
//   Serial.print(watchDuration);
//   Serial.println(F(" seconds."));
//   unsigned long timer = 0; // used to track seconds during sail operation
//   while (timer < watchDuration) {
//     GbUtility::GortoNap(1); // one second of napping
//     timer = timer + 1;
//     // TODO blinkMessage(2); // flash led
//     Serial.print(F("timer = "));
//     Serial.println(timer);
//     delay(10);
//   }
// }

// void GbWatchStander::PulseSail(GbSail sail) {
//   Serial.println(F("Pulsing sail."));
//   sail.TurnCW();
//   delay(1000);
//   sail.TurnCCW();
//   delay(1000);
//   sail.Stop();
// }
