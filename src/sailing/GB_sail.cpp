#include "gb_sail.h"

GbSail::GbSail(uint8_t sensorPin, uint8_t sensorEnablePin,
               uint8_t motorPowerEnablePin, uint8_t motorDirectionPin,
               uint8_t motorSpeedPin, int16_t min_sail_angle,
               int16_t max_sail_angle, uint16_t trimRoutineMaxSeconds,
               int16_t mastPositionCalibration)
    : _sensorPin(sensorPin), _sensorEnablePin(sensorEnablePin),
      _mastGearSize(48), _sensorGearSize(16),
      _motorPowerEnablePin(motorPowerEnablePin),
      _motorDirectionPin(motorDirectionPin), _motorSpeedPin(motorSpeedPin),
      _min_sail_angle(min_sail_angle), _max_sail_angle(max_sail_angle),
      _trimRoutineMaxSeconds(trimRoutineMaxSeconds),
      _mast_position_calibration(mastPositionCalibration) {
  pinMode(_sensorPin, INPUT);
  pinMode(_sensorEnablePin, OUTPUT);
  digitalWrite(_sensorEnablePin, LOW);
  pinMode(_motorPowerEnablePin, OUTPUT);
  digitalWrite(_motorPowerEnablePin, LOW);
  pinMode(_motorDirectionPin, OUTPUT);
  digitalWrite(_motorDirectionPin, LOW);
  pinMode(_motorSpeedPin, OUTPUT);
  digitalWrite(_motorSpeedPin, LOW);
}

GbTrimResult GbSail::Trim(int16_t orderedSailPosition) {
  uint32_t trimStartTime = millis();
  uint32_t lastMoveTime = trimStartTime;
  int16_t sailPosition = GetSailPosition();
  int16_t sailPositionBefore = sailPosition;
  bool sailIsTrimming = true;

  // Try to trim to the the ordered sail position
  while (!CloseEnough(sailPosition, orderedSailPosition) &&
         !TrimRoutineExceeded(trimStartTime) && sailIsTrimming &&
         ValidOrders(orderedSailPosition)) {

    TurnSailTowardsTarget(sailPosition, orderedSailPosition);

    if (CloserToTarget(sailPosition, sailPositionBefore, orderedSailPosition)) {
      sailIsTrimming = true;
      lastMoveTime = millis();
      sailPositionBefore = sailPosition;
    } else {
      if ((uint32_t)(millis() - lastMoveTime) > 10000) {
        sailIsTrimming = false;
      }
    }

    sailPosition = GetSailPosition() + 5; // correction for motor
  }

  Stop(); // sail is either in position or stuck

  GbTrimResult trimResult = {
      .success = CloseEnough(sailPosition, orderedSailPosition),
      .sailStuck = !sailIsTrimming,
      .trimRoutineExceededMax = TrimRoutineExceeded(trimStartTime),
      .sailBatteryTooLow = false};

  // OutputTrimResults(trimResult);
  return trimResult;
}

bool GbSail::CloserToTarget(int16_t sailPosition, int16_t sailPositionBefore,
                            int16_t orderedSailPosition) {
  return (abs(sailPosition - orderedSailPosition) <
          abs(sailPositionBefore - orderedSailPosition));
}

void GbSail::OutputTrimResults(GbTrimResult trimResult) {
  DEBUG_PRINT(F("Trim routine results: success = "));
  DEBUG_PRINT(trimResult.success);
  DEBUG_PRINT(F(" | sailStuck = "));
  DEBUG_PRINT(trimResult.sailStuck);
  DEBUG_PRINT(F(" | trimRoutineExceededMax = "));
  DEBUG_PRINT(trimResult.trimRoutineExceededMax);
  DEBUG_PRINT(F(" | sailBatteryTooLow = "));
  DEBUG_PRINTLN(trimResult.sailBatteryTooLow);
}

bool GbSail::CloseEnough(int16_t sailPosition, int16_t orderedSailPosition) {
  return abs(sailPosition - orderedSailPosition) < 2;
}

bool GbSail::TrimRoutineExceeded(uint32_t trimStartTime) {
  return ((uint32_t)(millis() - trimStartTime) / 1000 > _trimRoutineMaxSeconds);
}

void GbSail::TurnSailTowardsTarget(int16_t sailPosition,
                                   int16_t orderedSailPosition) {
  uint16_t outOfTrim = abs(sailPosition - orderedSailPosition);
  if (sailPosition < orderedSailPosition) {
    TurnCW(outOfTrim);
  } else if (sailPosition > orderedSailPosition) {
    TurnCCW(outOfTrim);
  } else {
    Stop();
  }
}

int16_t GbSail::GetSailPosition() {
  int16_t positionAnalogReading = GetPositionAnalogReading();
  float gearRatio = float(_mastGearSize) / _sensorGearSize;
  const int8_t TURNS_IN_POT = 10;
  float turnsInMast = TURNS_IN_POT / gearRatio;
  int16_t degreesInMast = turnsInMast * 360.0;
  int16_t positionDegrees =
      degreesInMast * float((positionAnalogReading / 1023.0)) -
      (degreesInMast / 2) + 180;
  positionDegrees += _mast_position_calibration;
  return positionDegrees;
}

int16_t GbSail::GetPositionAnalogReading() {
  const int8_t REPETITIONS = 20;
  int16_t sum = 0;
  digitalWrite(_sensorEnablePin, HIGH);
  for (int8_t i = 0; i < REPETITIONS; i++) {
    sum = sum + analogRead(_sensorPin);
    delay(10);
  }
  digitalWrite(_sensorEnablePin, LOW);
  int16_t averageAnalogReading = sum / REPETITIONS;
  return averageAnalogReading;
}

void GbSail::TurnCW(uint16_t outOfTrim) {
  digitalWrite(_motorPowerEnablePin, HIGH);
  digitalWrite(_motorDirectionPin, LOW);
  digitalWrite(_motorSpeedPin, HIGH);
}

void GbSail::TurnCCW(uint16_t outOfTrim) {
  digitalWrite(_motorPowerEnablePin, HIGH);
  digitalWrite(_motorDirectionPin, HIGH);
  digitalWrite(_motorSpeedPin, HIGH);
}

void GbSail::Stop() {
  digitalWrite(_motorPowerEnablePin, LOW);
  digitalWrite(_motorDirectionPin, LOW);
  digitalWrite(_motorSpeedPin, LOW);
}

bool GbSail::ValidOrders(int16_t order) {
  if ((order > _max_sail_angle) || (order < _min_sail_angle)) {
    return false; // out-of-bouds
  } else {
    return true;
  }
}
