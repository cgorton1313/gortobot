#include <Arduino.h>
#include <power/Battery.h>
#include <Narcoleptic.h>

Battery::Battery(byte pin, float minVoltage, float retryVoltage, int waitTime, bool checkingVoltage) {
    _pin = pin;
    _minVoltage = minVoltage;
    _retryVoltage = retryVoltage;
    _waitTime = waitTime;
    _checkingVoltage = checkingVoltage;
}

float Battery::GetVoltage() {
    if (_checkingVoltage) {
        int batteryVoltageInt = 0;
        const int samples = 10; // number of samples to take
        for (int i = 0; i < samples; i++) {
                delay(5);
                batteryVoltageInt = batteryVoltageInt + analogRead(_pin);
        }
        return (4.096 * (((float)batteryVoltageInt / samples) / 1023.0));
    }
    else {
        return 3.99;
    }
}

void Battery::Okay() {
    float voltageNow;
    bool voltageCritical = false;

    if (_checkingVoltage) {
        voltageNow = this->GetVoltage();
        Serial.print(F("Voltage = "));
    }
    else {
        voltageNow = 3.99;
        Serial.print(F("Fake voltage = "));
    }
    Serial.println(voltageNow);

    if (voltageNow < _minVoltage) {
            voltageCritical = true;
    }
    while (voltageCritical) {
            Serial.print(F("Voltage critical! Voltage = "));
            Serial.print(voltageNow);
            Serial.print(F(". Waiting "));
            Serial.print(_waitTime);
            Serial.println(F(" seconds."));
            for (int i = 0; i < _waitTime; i++) {
                    Narcoleptic.delay(1000);
            }
            voltageNow = this->GetVoltage();
            if (voltageNow > _retryVoltage) {
                    voltageCritical = false;
            }
    }
    Serial.print(F("Voltage okay. Voltage = "));
    Serial.println(voltageNow);
}
