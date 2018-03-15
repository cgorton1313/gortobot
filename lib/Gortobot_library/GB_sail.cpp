#include "gb_sail.h"

GbSail::GbSail(byte sensorPin, byte sensorEnablePin, byte motorPowerEnablePin,
               byte motorIn1Pin, byte motorIn2Pin, int min_sail_angle, int max_sail_angle) {
  _sensorEnablePin = sensorEnablePin;
  _sensorPin = sensorPin;
  _motorPowerEnablePin = motorPowerEnablePin;
  _motorIn1Pin = motorIn1Pin;
  _motorIn2Pin = motorIn2Pin;
  _min_sail_angle = min_sail_angle;
  _max_sail_angle = max_sail_angle;
  pinMode(_sensorPin, INPUT);
  pinMode(_sensorEnablePin, OUTPUT);
  pinMode(_motorPowerEnablePin, OUTPUT);
  digitalWrite(_motorPowerEnablePin, LOW);
  pinMode(_motorIn1Pin, OUTPUT);
  pinMode(_motorIn2Pin, OUTPUT);
  _mastGearSize = 74;
  _sensorGearSize = 36;
}

void GbSail::Trim(int orderedSailPosition) {
  int currentPosition = GetPosition();
  if (abs(currentPosition - orderedSailPosition) >= 5) // sail is out of trim
  {
    Serial.println("Sail is OUT of trim");
    digitalWrite(_motorPowerEnablePin, HIGH);
    delay(500);
    while (currentPosition != orderedSailPosition) {
      digitalWrite(_motorIn1Pin, currentPosition > orderedSailPosition);
      digitalWrite(_motorIn2Pin, currentPosition < orderedSailPosition);
      currentPosition = GetPosition();
      Serial.println(currentPosition);
      delay(50);
    }
  }
  digitalWrite(_motorPowerEnablePin, LOW);
  digitalWrite(_motorIn1Pin, LOW);
  digitalWrite(_motorIn2Pin, LOW);
}

int GbSail::GetPosition() {
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
