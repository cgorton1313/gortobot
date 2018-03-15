#include <Arduino.h> // for Stream

#ifndef GB_SAIL_H
#define GB_SAIL_H

class GbSail {
public:
  GbSail(byte sensorPin, byte sensorEnablePin, byte motorPowerEnablePin,
       byte motorIn1Pin, byte motorIn2Pin, int min_sail_angle, int max_sail_angle);
  void Trim(int angle);
  int GetPosition();
  void TurnCW();
  void TurnCCW();
  void Stop();
  boolean ValidOrders(int order);

private:
  byte _sensorPin;
  byte _sensorEnablePin;
  byte _mastGearSize;
  byte _sensorGearSize;
  byte _motorPowerEnablePin;
  byte _motorIn1Pin;
  byte _motorIn2Pin;
  int _min_sail_angle;
  int _max_sail_angle;
  int GetPositionAnalogReading();
};

#endif
