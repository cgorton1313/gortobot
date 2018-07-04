/*
The sail class sets up and controls the sail.
*/

#include "gb_trim_result.h"
#include "../../src/configs/config.h"
#include <Arduino.h> // for Stream
#include <Vcc.h>

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
  const int16_t _min_sail_angle;
  const int16_t _max_sail_angle;
  const uint16_t _trimRoutineMaxSeconds;
  uint16_t GetPositionAnalogReading();
  bool CloseEnough(int16_t sailPosition, int16_t orderedSailPosition);
  bool CloserToTarget(int16_t sailPosition, int16_t sailPositionBefore,
                      int16_t orderedSailPosition);
  bool TrimRoutineExceeded(uint32_t trimStartTime);
  void TurnSailTowardsTarget(int16_t sailPosition, int16_t orderedSailPosition);
  void TurnCW();
  void TurnCCW();
  void Stop();

public:
  GbSail(uint8_t sensorPin, uint8_t sensorEnablePin,
         uint8_t motorPowerEnablePin, uint8_t motorDirectionPin,
         uint8_t motorSpeedPin, int16_t min_sail_angle, int16_t max_sail_angle,
         uint16_t trimRoutineMaxSeconds);
  void OutputTrimResults(GbTrimResult trimResult);
  GbTrimResult Trim(int16_t angle);
  int16_t GetSailPosition();
  bool ValidOrders(int16_t order);
};

#endif
