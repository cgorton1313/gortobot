#include "gb_blinker.h"

GbBlinker::GbBlinker(uint8_t pin) : _ledPin(pin) {}

void GbBlinker::Blink(uint16_t condition) {
  switch (condition) {
  case 1: // continuous quick flash, stops program here
    while (true) {
      digitalWrite(_ledPin,
                   (millis() / 100) % 2 == 1 ? HIGH : LOW); // rapid blink
    }
    break;
  case 2: // single quick flash
    digitalWrite(_ledPin, HIGH);
    delay(20);
    digitalWrite(_ledPin, LOW);
    break;
  case 3: // continuous on 2, off half, program stops here
    while (true) {
      digitalWrite(_ledPin, HIGH);
      delay(2000);
      digitalWrite(_ledPin, LOW);
      delay(500);
    }
    break;
  }
}
