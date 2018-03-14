#include "gb_watch_stander.h"
#include "utilities/gb_utility.h"

GbWatchStander::GbWatchStander() {}

void GbWatchStander::StandWatch(GbSail sail, GbSailingOrders sailingOrders) {
  switch (sailingOrders.sailMode) {
  case 'r': // real sailing
    Serial.println("Entering real sail mode.");
    // TODO realSail();
    break;
  case 't': // testing sail
    Serial.println(F("Entering test sail mode."));
    // TODO testSail();
    break;
  case 's': // direct sets
    Serial.println(F("Entering direct set sail mode."));
    // TODO directSetSail();
    break;
  case 'p': // pulse
    Serial.println(F("Entering pulsing sail mode."));
    PulseSail(sail);
    break;
  case 'f': // pulse
    Serial.println(F("Entering fake sail mode."));
    FakeSail(sailingOrders.loggingInterval);
    break;
  }
}

void GbWatchStander::PulseSail(GbSail sail) {
  Serial.println(F("Pulsing sail."));
  sail.TurnCW();
  delay(1000);
  sail.TurnCCW();
  delay(1000);
  sail.Stop();
}

void GbWatchStander::FakeSail(unsigned long watchDuration) {
  Serial.print(F("Fake sailing for "));
  Serial.print(watchDuration);
  Serial.println(F(" seconds."));
  unsigned long timer = 0; // used to track seconds during sail operation
  while (timer < watchDuration) {
    GbUtility::GortoNap(1); // one second of napping
    timer = timer + 1;
    // TODO blinkMessage(2); // flash led
    Serial.print(F("timer = "));
    Serial.println(timer);
    delay(10);
  }
}
