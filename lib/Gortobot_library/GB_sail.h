#include <Arduino.h> // for Stream

#ifndef GB_SAIL_H
#define GB_SAIL_H

class Sail
{
    public:
        Sail(byte sensorPin, byte sensorEnablePin);
        void Trim(int angle);
        int GetPosition();

    private:
        byte _sensorPin;
        byte _sensorEnablePin;
        byte _mastGearSize;
        byte _sensorGearSize;
        int GetPositionAnalogReading();
};

#endif
