#include <Arduino.h>

class Battery {

private:
    float _voltage;
    float _minVoltage;
    float _retryVoltage;
    int _waitTime;

public:
    Battery(float minVoltage, float retryVoltage, int waitTime);
    float getVoltage();
};
