#include <Arduino.h>
#include <power/Battery.h>

Battery::Battery(byte pin, float minVoltage, float retryVoltage, int waitTime, bool checkingVoltage) {
    _pin = pin;
    _minVoltage = minVoltage;
    _retryVoltage = retryVoltage;
    _waitTime = waitTime;
    _checkingVoltage = checkingVoltage;
}

float Battery::getVoltage() {
    if (_checkingVoltage) {
        int batteryVoltageInt = 0;
        float correction = 1;
        const int samples = 10; // number of samples to take
        for (int i = 0; i < samples; i++) {
                delay(5);
                batteryVoltageInt = batteryVoltageInt + analogRead(_pin);
        }
        return (5.08 * correction * (((float)batteryVoltageInt / samples) / 1023.0));
    }
    else {
        return 3.99;
    }
}

void Battery::okay() {
    Serial.print(F("Voltage = "));
    Serial.println(this->getVoltage());
    // wait until okay
}
