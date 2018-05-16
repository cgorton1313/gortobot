#include "gb_sail.h"

GbSail::GbSail(uint8_t sensorPin, uint8_t sensorEnablePin,
               uint8_t motorPowerEnablePin, uint8_t motorDirectionPin,
               uint8_t motorSpeedPin, uint16_t min_sail_angle,
               uint16_t max_sail_angle, uint16_t trimRoutineMaxSeconds)
    : _sensorPin(sensorPin), _sensorEnablePin(sensorEnablePin),
      _mastGearSize(74), _sensorGearSize(36),
      _motorPowerEnablePin(motorPowerEnablePin), _motorDirectionPin(motorDirectionPin),
      _motorSpeedPin(motorSpeedPin), _min_sail_angle(min_sail_angle),
      _max_sail_angle(max_sail_angle),
      _trimRoutineMaxSeconds(trimRoutineMaxSeconds) {
  pinMode(_sensorPin, INPUT);
  pinMode(_sensorEnablePin, OUTPUT);
  pinMode(_motorPowerEnablePin, OUTPUT);
  digitalWrite(_motorPowerEnablePin, LOW);
  pinMode(_motorSpeedPin, OUTPUT);
  pinMode(_motorSpeedPin, OUTPUT);
}

GbTrimResult GbSail::Trim(uint16_t orderedSailPosition) {
  uint32_t trimStartTime = millis();
  uint32_t lastMoveTime = trimStartTime;
  uint16_t sailPosition = GetSailPosition();
  uint16_t sailPositionBefore = sailPosition;
  bool sailIsTrimming = true;

  // Try to trim to the the ordered sail position
  while (!CloseEnough(sailPosition, orderedSailPosition) &&
         !TrimRoutineExceeded(trimStartTime) && sailIsTrimming) {
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

    sailPosition = GetSailPosition();

  }

  Stop(); // sail is either in position or stuck
  GbTrimResult trimResult = {
      .success = CloseEnough(sailPosition, orderedSailPosition),
      .sailStuck = !sailIsTrimming,
      .trimRoutineExceededMax = TrimRoutineExceeded(trimStartTime),
      .sailBatteryTooLow = false};

  OutputTrimResults(trimResult);
  return trimResult;
}

bool GbSail::CloserToTarget(uint16_t sailPosition, uint16_t sailPositionBefore,
                            uint16_t orderedSailPosition) {
  return (abs((int16_t)(sailPosition - orderedSailPosition)) <
          abs((int16_t)(sailPositionBefore - orderedSailPosition)));
}

void GbSail::OutputTrimResults(GbTrimResult trimResult) {
  Serial.print(F("Trim routine results: success = "));
  Serial.print(trimResult.success);
  Serial.print(F(" | sailStuck = "));
  Serial.print(trimResult.sailStuck);
  Serial.print(F(" | trimRoutineExceededMax = "));
  Serial.print(trimResult.trimRoutineExceededMax);
  Serial.print(F(" | sailBatteryTooLow = "));
  Serial.println(trimResult.sailBatteryTooLow);
}

bool GbSail::CloseEnough(uint16_t sailPosition, uint16_t orderedSailPosition) {
  return abs((int16_t)(sailPosition - orderedSailPosition)) < 2;
}

bool GbSail::TrimRoutineExceeded(uint32_t trimStartTime) {
  return ((uint32_t)(millis() - trimStartTime) / 1000 > _trimRoutineMaxSeconds);
}

void GbSail::TurnSailTowardsTarget(uint16_t sailPosition,
                                   uint16_t orderedSailPosition) {
  if (sailPosition < orderedSailPosition) {
    TurnCW();
  } else if (sailPosition > orderedSailPosition) {
    TurnCCW();
  } else {
    Stop();
  }
}

uint16_t GbSail::GetSailPosition() {
  uint16_t positionAnalogReading = GetPositionAnalogReading();
  float gearRatio = float(_mastGearSize) / _sensorGearSize;
  const uint8_t TURNS_IN_POT = 10;
  float readingRange = (float(gearRatio) / TURNS_IN_POT *
                        1023.0); // 1 mast turn = 1 * gearRatio pot turns
  const uint16_t CENTER_READING = 512;
  uint16_t minReading = int(CENTER_READING - (readingRange / 2.0));
  uint16_t maxReading = int(CENTER_READING + (readingRange / 2.0));
  uint16_t positionDegrees =
      map(positionAnalogReading, minReading, maxReading, 1, 359);
  return positionDegrees;
}

uint16_t GbSail::GetPositionAnalogReading() {
  digitalWrite(_sensorEnablePin, HIGH);
  uint16_t position;
  const uint8_t REPETITIONS = 20;
  uint16_t sum = 0;
  for (uint8_t i = 0; i < REPETITIONS; i++) {
    sum = sum + analogRead(_sensorPin);
    delay(1);
  }
  position = sum / REPETITIONS;
  return position;
}

void GbSail::TurnCW() {
  digitalWrite(_motorPowerEnablePin, HIGH);
  digitalWrite(_motorDirectionPin, LOW);
  digitalWrite(_motorSpeedPin, HIGH);
}

void GbSail::TurnCCW() {
  digitalWrite(_motorPowerEnablePin, HIGH);
  digitalWrite(_motorDirectionPin, HIGH);
  digitalWrite(_motorSpeedPin, HIGH);
}

void GbSail::Stop() {
  digitalWrite(_motorPowerEnablePin, LOW);
  digitalWrite(_motorDirectionPin, LOW);
  digitalWrite(_motorSpeedPin, LOW);
}

bool GbSail::ValidOrders(uint16_t order) {
  if ((order > _max_sail_angle) || (order < _min_sail_angle)) {
    return false; // out-of-bouds
  } else {
    return true;
  }
}
