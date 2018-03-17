#include <Arduino.h>

#ifndef GB_MESSAGE_HANDLER_H
#define GB_MESSAGE_HANDLER_H

class GbBlinker {

private:
  uint8_t _ledPin;

public:
  GbBlinker(uint8_t pin);
  void SetLedPin(uint8_t pin);
  void Blink(uint16_t condition);
};

#endif
