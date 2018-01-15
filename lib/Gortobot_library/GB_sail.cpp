#include "gb_sail.h"

Sail::Sail(byte sensorPin, byte sensorEnablePin, byte motorPowerEnablePin,
           byte motorIn1Pin, byte motorIn2Pin)
{
        _sensorEnablePin = sensorEnablePin;
        _sensorPin = sensorPin;
        _motorPowerEnablePin = motorPowerEnablePin;
        _motorIn1Pin = motorIn1Pin;
        _motorIn2Pin = motorIn2Pin;
        pinMode(_sensorPin, INPUT);
        pinMode(_sensorEnablePin, OUTPUT);
        pinMode(_motorPowerEnablePin, OUTPUT);
        digitalWrite(_motorPowerEnablePin, LOW);
        pinMode(_motorIn1Pin, OUTPUT);
        pinMode(_motorIn2Pin, OUTPUT);
        _mastGearSize = 74;
        _sensorGearSize = 36;
}

void Sail::Trim(int orderedSailPosition)
{
        int currentPosition = GetPosition();
        if (abs(currentPosition - orderedSailPosition) >= 5) // sail is out of trim
        {
                Serial.println("Sail is OUT of trim");
                digitalWrite(_motorPowerEnablePin, HIGH);
                delay(500);
                while (currentPosition != orderedSailPosition)
                {
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
                delay(1);
        }
        position = sum / REPETITIONS;
        return position;
}
