#include <Arduino.h>

#ifndef GB_BLINKER_H
#define GB_BLINKER_H

class GbBlinker {

private:
  uint8_t _ledPin;

public:
  GbBlinker(uint8_t pin);
  void SetLedPin(uint8_t pin);
  void Blink(uint16_t condition);
};

#endif
