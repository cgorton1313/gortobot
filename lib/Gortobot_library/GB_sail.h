#include <Arduino.h> // for Stream

#ifndef GB_SAIL_H
#define GB_SAIL_H

class Sail
{
    public:
        Sail(byte sensorPin, byte sensorEnablePin, byte motorPowerEnablePin,
                    byte motorIn1Pin, byte motorIn2Pin);
        void Trim(int angle);
        int GetPosition();

    private:
        byte _sensorPin;
        byte _sensorEnablePin;
        byte _mastGearSize;
        byte _sensorGearSize;
        byte _motorPowerEnablePin;
        byte _motorIn1Pin;
        byte _motorIn2Pin;
        int GetPositionAnalogReading();
};

#endif
