#include <Arduino.h>

#ifndef GB_MESSAGE_HANDLER_H
#define GB_MESSAGE_HANDLER_H

class GbBlinker {

private:
  byte _ledPin;

public:
  GbBlinker(byte pin);
  void SetLedPin(byte pin);
  void Blink(int condition);
};

#endif
