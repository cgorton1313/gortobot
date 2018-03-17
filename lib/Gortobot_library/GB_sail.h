#include <Arduino.h> // for Stream

#ifndef GB_SAIL_H
#define GB_SAIL_H

class GbSail {
public:
  GbSail(uint8_t sensorPin, uint8_t sensorEnablePin, uint8_t motorPowerEnablePin,
         uint8_t motorIn1Pin, uint8_t motorIn2Pin, uint16_t min_sail_angle,
         uint16_t max_sail_angle, uint16_t trimRoutineMaxSeconds);
  struct GbTrimResult {
    bool success;
    bool sailNotMoving;
    bool trimRoutineExceededMax;
  };
  GbTrimResult Trim(uint16_t angle);
  uint16_t GetSailPosition();
  void TurnCW();
  void TurnCCW();
  void Stop();
  bool ValidOrders(uint16_t order);

private:
  uint8_t _sensorPin;
  uint8_t _sensorEnablePin;
  uint8_t _mastGearSize;
  uint8_t _sensorGearSize;
  uint8_t _motorPowerEnablePin;
  uint8_t _motorIn1Pin;
  uint8_t _motorIn2Pin;
  uint16_t _min_sail_angle;
  uint16_t _max_sail_angle;
  uint16_t _trimRoutineMaxSeconds;
  uint16_t GetPositionAnalogReading();
};

#endif
