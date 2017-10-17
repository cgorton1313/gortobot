#include <Arduino.h>
#include <power/Battery.h>

Battery::Battery(float minVoltage, float retryVoltage, int waitTime) {
    _minVoltage = minVoltage;
    _retryVoltage = retryVoltage;
    _waitTime = waitTime;
}

float Battery::getVoltage() {
    return 0.0;
}
