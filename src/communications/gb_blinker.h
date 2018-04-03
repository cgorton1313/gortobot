/*
   The GbBlinker is used to communicate Gortobot's status via the LED.
   Useful for figuring out what it's doing during backyard tests
*/

#include <Arduino.h>

#ifndef GB_BLINKER_H
#define GB_BLINKER_H

class GbBlinker {

private:
  const uint8_t _ledPin;

public:
  GbBlinker(uint8_t pin);
  void SetLedPin(uint8_t pin);
  void Blink(uint16_t condition);
};

#endif
