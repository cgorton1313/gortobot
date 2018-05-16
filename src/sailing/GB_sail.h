/*
The sail class sets up and controls the sail.
*/

#include "gb_trim_result.h"
#include <Arduino.h> // for Stream

#ifndef GB_SAIL_H
#define GB_SAIL_H

class GbSail {

private:
  const uint8_t _sensorPin;
  const uint8_t _sensorEnablePin;
  const uint8_t _mastGearSize;
  const uint8_t _sensorGearSize;
  const uint8_t _motorPowerEnablePin;
  const uint8_t _motorDirectionPin;
  const uint8_t _motorSpeedPin;
  const uint16_t _min_sail_angle;
  const uint16_t _max_sail_angle;
  const uint16_t _trimRoutineMaxSeconds;
  uint16_t GetPositionAnalogReading();
  bool CloseEnough(uint16_t sailPosition, uint16_t orderedSailPosition);
  bool CloserToTarget(uint16_t sailPosition, uint16_t sailPositionBefore,
                      uint16_t orderedSailPosition);
  bool TrimRoutineExceeded(uint32_t trimStartTime);
  void TurnSailTowardsTarget(uint16_t sailPosition,
                             uint16_t orderedSailPosition);
  void TurnCW();
  void TurnCCW();
  void Stop();

public:
  GbSail(uint8_t sensorPin, uint8_t sensorEnablePin,
         uint8_t motorPowerEnablePin, uint8_t motorDirectionPin, uint8_t motorSpeedPin,
         uint16_t min_sail_angle, uint16_t max_sail_angle,
         uint16_t trimRoutineMaxSeconds);
  void OutputTrimResults(GbTrimResult trimResult);
  GbTrimResult Trim(uint16_t angle);
  uint16_t GetSailPosition();
  bool ValidOrders(uint16_t order);
};

#endif
