#include "GB_sail.h"

Sail::Sail(byte sensorPin, byte sensorEnablePin)
{
    _sensorEnablePin = sensorEnablePin;
    _sensorPin = sensorPin;
    pinMode(_sensorPin, INPUT);
    pinMode(_sensorEnablePin, OUTPUT);
    _mastGearSize = 74;
    _sensorGearSize = 36;
}

void Sail::Trim(int angle)
{
    Serial.println("Trim function called");
}

int Sail::GetPosition()
{
    int positionAnalogReading = GetPositionAnalogReading();
    float gearRatio = float(_mastGearSize) / _sensorGearSize;
    const byte TURNS_IN_POT = 10;
    float readingRange = (float(gearRatio) / TURNS_IN_POT * 1023.0); // 1 mast turn = 1 * gearRatio pot turns
    const int CENTER_READING = 512;
    int minReading = int(CENTER_READING - (readingRange / 2.0));
    int maxReading = int(CENTER_READING + (readingRange / 2.0));
    int positionDegrees = map(positionAnalogReading, minReading, maxReading, 1, 359);
    return positionDegrees;
}

int Sail::GetPositionAnalogReading()
{
    digitalWrite(_sensorEnablePin, HIGH);
    int position;
    const byte REPETITIONS = 10;
    int sum = 0;
    for (byte i = 0; i < REPETITIONS; i++) {
            sum = sum + analogRead(_sensorPin);
            delay(10);
    }
    position = sum / REPETITIONS;
    return position;
}
