#include "gb_sail.h"

GbSail::GbSail(byte sensorPin, byte sensorEnablePin, byte motorPowerEnablePin,
               byte motorIn1Pin, byte motorIn2Pin, int min_sail_angle,
               int max_sail_angle, int trimRoutineMaxSeconds) {
  _sensorEnablePin = sensorEnablePin;
  _sensorPin = sensorPin;
  _motorPowerEnablePin = motorPowerEnablePin;
  _motorIn1Pin = motorIn1Pin;
  _motorIn2Pin = motorIn2Pin;
  _min_sail_angle = min_sail_angle;
  _max_sail_angle = max_sail_angle;
  _trimRoutineMaxSeconds = trimRoutineMaxSeconds;
  pinMode(_sensorPin, INPUT);
  pinMode(_sensorEnablePin, OUTPUT);
  pinMode(_motorPowerEnablePin, OUTPUT);
  digitalWrite(_motorPowerEnablePin, LOW);
  pinMode(_motorIn1Pin, OUTPUT);
  pinMode(_motorIn2Pin, OUTPUT);
  _mastGearSize = 74;
  _sensorGearSize = 36;
}

GbSail::GbTrimResult GbSail::Trim(int orderedSailPosition) {
  unsigned long trimTimer;
  int trimSeconds = 0;
  int totalTrimSeconds = 0;
  bool sailIsTrimming = true;

  int sailPosition = GetSailPosition();
  int tempSailPosition = sailPosition;
  bool trimRoutineExceededMax = false;
  bool sailNotMoving = false;

  if ((abs(GetSailPosition() - orderedSailPosition)) > 4) { // sail out of trim
    trimTimer = millis(); // capture the time now
    while (((sailPosition - orderedSailPosition) != 0) && sailIsTrimming) {

      // Start rotating sail
      digitalWrite(_motorIn1Pin, sailPosition > orderedSailPosition);
      digitalWrite(_motorIn2Pin, sailPosition < orderedSailPosition);

      // Every second, check if we're done, if we timed out or sail stopped
      // moving
      if ((unsigned long)(millis() - trimTimer) >= 1000) {
        trimTimer = millis(); // reset timer
        totalTrimSeconds++;
        if (totalTrimSeconds > _trimRoutineMaxSeconds) {
          sailIsTrimming = false;
          trimRoutineExceededMax = true;
          Serial.println(F("trimRoutineExceededMax"));
        }
        sailPosition = this->GetSailPosition();
        if (sailPosition == tempSailPosition) {
          // sail hasn't moved in the last second
          trimSeconds++;
          if (trimSeconds > 10) { // sail hasn't moved in 10 seconds, bad!
            sailIsTrimming = false;
            sailNotMoving = true;
            Serial.println(F("Sail not moving")); // what do we do? diags!
          }
        } else { // sail has moved
          tempSailPosition = sailPosition;
          trimSeconds = 0;
          sailNotMoving = false;
        }
      }
    }
    Stop(); // sail is either in position or stuck
  }
  Serial.print(F("Done trimming. totalTrimSeconds = "));
  Serial.println(totalTrimSeconds);

  bool trimSuccess = (!sailNotMoving && !trimRoutineExceededMax);
  GbTrimResult trimResult = {.success = trimSuccess,
                             .sailNotMoving = sailNotMoving,
                             .trimRoutineExceededMax = trimRoutineExceededMax};

  return trimResult;
}

int GbSail::GetSailPosition() {
  int positionAnalogReading = GetPositionAnalogReading();
  float gearRatio = float(_mastGearSize) / _sensorGearSize;
  const byte TURNS_IN_POT = 10;
  float readingRange = (float(gearRatio) / TURNS_IN_POT *
                        1023.0); // 1 mast turn = 1 * gearRatio pot turns
  const int CENTER_READING = 512;
  int minReading = int(CENTER_READING - (readingRange / 2.0));
  int maxReading = int(CENTER_READING + (readingRange / 2.0));
  int positionDegrees =
      map(positionAnalogReading, minReading, maxReading, 1, 359);
  return positionDegrees;
}

int GbSail::GetPositionAnalogReading() {
  digitalWrite(_sensorEnablePin, HIGH);
  int position;
  const byte REPETITIONS = 10;
  int sum = 0;
  for (byte i = 0; i < REPETITIONS; i++) {
    sum = sum + analogRead(_sensorPin);
    delay(1);
  }
  position = sum / REPETITIONS;
  return position;
}

void GbSail::TurnCW() {
  digitalWrite(_motorPowerEnablePin, LOW);
  digitalWrite(_motorIn1Pin, HIGH);
  digitalWrite(_motorIn2Pin, LOW);
}

void GbSail::TurnCCW() {
  digitalWrite(_motorPowerEnablePin, LOW);
  digitalWrite(_motorIn1Pin, LOW);
  digitalWrite(_motorIn2Pin, HIGH);
}

void GbSail::Stop() {
  Serial.println(F("Stopping the sail motor."));
  digitalWrite(_motorPowerEnablePin, HIGH);
  digitalWrite(_motorIn1Pin, LOW);
  digitalWrite(_motorIn2Pin, LOW);
}

boolean GbSail::ValidOrders(int order) {
  if ((order > _max_sail_angle) || (order < _min_sail_angle)) {
    return false; // out-of-bouds
  } else {
    return true;
  }
}
