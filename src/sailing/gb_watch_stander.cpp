#include "gb_watch_stander.h"
#include "utilities/gb_utility.h"
#include "communications/gb_blinker.h"

GbWatchStander::GbWatchStander(uint8_t ledPin) : _blinker(ledPin) {
  // Start on tack A at time 0
  _tackIsA = true;                    // am I on tack A?
  _currentTackTime = 0;               // how minutes have I been on this tack?
  _current_orderedSailPosition = 270; // TODO
}

void GbWatchStander::StandWatch(GbSail sail, GbSailingOrders sailingOrders) {
    Serial.println("Entering real sail mode.");
    RealSail(sail, sailingOrders);
}

void GbWatchStander::RealSail(GbSail sail, GbSailingOrders sailingOrders) {
  Serial.print(F("Sailing for "));
  Serial.print(sailingOrders.loggingInterval);
  Serial.println(F(" seconds."));

  unsigned long elapsedTime = 0; // used to track seconds during sail operation
  GbSail::GbTrimResult trimResult;
  while (elapsedTime < sailingOrders.loggingInterval) {

    // Every minute, check sail trim angle and whether it is time to tack
    if ((elapsedTime % 60) == 0) {
      Serial.println(F("At :00 seconds now"));
      if ((_tackIsA && _currentTackTime >= sailingOrders.orderedTackTimeA) ||
          (!_tackIsA && _currentTackTime >= sailingOrders.orderedTackTimeB)) {
        _tackIsA = !_tackIsA; // change tacks
        _currentTackTime = 0;
        Serial.println(F("Changing tacks"));
      }

      // Set the current ordered sail angle per which tack we're on
      if (_tackIsA) {
        _current_orderedSailPosition = sailingOrders.orderedSailPositionA;
        Serial.println(F("Sailing on tack A"));
      } else {
        _current_orderedSailPosition = sailingOrders.orderedSailPositionB;
        Serial.println(F("Sailing on tack B"));
      }

      if (sail.ValidOrders(1)) {
        trimResult = sail.Trim(_current_orderedSailPosition);
        // pruint16_t trim result for debug
      }

      _currentTackTime++;
      Serial.print(F("Current tack time = "));
      Serial.println(_currentTackTime);
    }

    GbUtility::GortoNap(6); // six seconds of napping
    elapsedTime += 6;
    // blinkMessage(2); // flash led
    Serial.print(F("timer = "));
    Serial.println(elapsedTime);
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

// Use TestSail to cycle thru sail positions per loggingInterval
void GbWatchStander::TestSail(GbSail sail, GbSailingOrders sailingOrders) {
  uint16_t testSailPositions[] = {0,   30,  60,  90,  120, 150, 180,
                             210, 240, 270, 300, 330, 360};
  for (uint8_t i = 0; i < (sizeof(testSailPositions) / sizeof(int)); i++) {
    sail.Trim(testSailPositions[i]);
    unsigned long testTimer = 0;
    while (testTimer <
           sailingOrders.loggingInterval) { // so the duration can be set via RX
      GbUtility::GortoNap(1);               // one second of napping
      testTimer = testTimer + 1;
      // blinkMessage(2); // flash led
      Serial.print(F("timer = "));
      Serial.println(testTimer);
    }
  }
}
