#include <Arduino.h>

class Battery {

private:
    byte _pin;
    float _voltage;
    float _minVoltage;
    float _retryVoltage;
    int _waitTime;
    bool _checkingVoltage;

public:
    Battery(byte pin, float minVoltage, float retryVoltage, int waitTime, bool checkingVoltage);
    float GetVoltage();
    void Okay();
};
